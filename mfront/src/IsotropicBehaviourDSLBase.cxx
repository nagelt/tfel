/*!
 * \file   mfront/src/IsotropicBehaviourDSLBase.cxx
 * \brief
 *
 * \author Thomas Helfer
 * \date   01 jui 2007
 * \copyright Copyright (C) 2006-2018 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence with
 * linking exception or the CECILL-A licence. A copy of thoses licences are
 * delivered with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 */

#include <vector>
#include <sstream>
#include "TFEL/Raise.hxx"
#include "TFEL/Glossary/Glossary.hxx"
#include "TFEL/Glossary/GlossaryEntry.hxx"
#include "TFEL/Utilities/StringAlgorithms.hxx"
#include "MFront/MFrontLogStream.hxx"
#include "MFront/MFrontWarningMode.hxx"
#include "MFront/DSLUtilities.hxx"
#include "MFront/IsotropicBehaviourDSLBase.hxx"

namespace mfront {

  IsotropicBehaviourDSLBase::IsotropicBehaviourDSLBase(const DSLOptions& opts)
      : BehaviourDSLBase<IsotropicBehaviourDSLBase>(opts) {
    constexpr auto h = ModellingHypothesis::UNDEFINEDHYPOTHESIS;
    this->reserveName("NewtonIntegration");
    // main variables
    this->mb.declareAsASmallStrainStandardBehaviour();
    // intermediate temperature
    const auto* const Topt = BehaviourDescription::
        automaticDeclarationOfTheTemperatureAsFirstExternalStateVariable;
    if (this->mb.getAttribute<bool>(Topt)) {
      // temperature at the midle of the time step
      const auto T_ = VariableDescription("temperature", "T_", 1u, 0u);
      this->mb.addLocalVariable(h, T_);
    }
    // material symmetry
    this->mb.setSymmetryType(mfront::ISOTROPIC);
    this->mb.setElasticSymmetryType(mfront::ISOTROPIC);
    // parameters
    this->mb.registerMemberName(h, "epsilon");
    this->mb.registerMemberName(h, "\u03B5");
    this->mb.registerMemberName(h, "theta");
    this->mb.registerMemberName(h, "\u03B8");
    this->mb.registerMemberName(h, "iterMax");
    // Call Back
    this->registerNewCallBack(
        "@UsableInPurelyImplicitResolution",
        &IsotropicBehaviourDSLBase::treatUsableInPurelyImplicitResolution);
    this->registerNewCallBack("@MaterialLaw",
                              &IsotropicBehaviourDSLBase::treatMaterialLaw);
    this->registerNewCallBack("@FlowRule",
                              &IsotropicBehaviourDSLBase::treatFlowRule);
    this->registerNewCallBack("@Theta", &IsotropicBehaviourDSLBase::treatTheta);
    this->registerNewCallBack("@Epsilon",
                              &IsotropicBehaviourDSLBase::treatEpsilon);
    this->registerNewCallBack("@IterMax",
                              &IsotropicBehaviourDSLBase::treatIterMax);
    this->registerNewCallBack(
        "@ElasticMaterialProperties",
        &IsotropicBehaviourDSLBase::treatElasticMaterialProperties);
    this->disableCallBack("@Brick");
    this->disableCallBack("@StateVar");
    this->disableCallBack("@StateVariable");
    this->disableCallBack("@Integrator");
    this->disableCallBack("@OrthotropicBehaviour");
    this->disableCallBack("@IsotropicElasticBehaviour");
    this->disableCallBack("@IsotropicBehaviour");
    this->disableCallBack("@RequireStiffnessOperator");
    this->disableCallBack("@RequireThermalExpansionCoefficientTensor");
    this->disableCallBack("@CrystalStructure");
    this->disableCallBack("@SlipSystem");
    this->disableCallBack("@GlidingSystem");
    this->disableCallBack("@SlidingSystem");
    this->disableCallBack("@SlipSystems");
    this->disableCallBack("@GlidingSystems");
    this->disableCallBack("@SlidingSystems");
    this->disableCallBack("@InteractionMatrix");
    // a defaut version of the prediction operator is always provided
    this->mb.setAttribute(h, BehaviourData::hasPredictionOperator, true);
    this->mb.setIntegrationScheme(BehaviourDescription::SPECIFICSCHEME);
  }  // end of IsotropicBehaviourDSLBase

  BehaviourDSLDescription
  IsotropicBehaviourDSLBase::getBehaviourDSLDescription() const {
    auto d = BehaviourDSLDescription{};
    d.behaviourType =
        tfel::material::MechanicalBehaviourBase::STANDARDSTRAINBASEDBEHAVIOUR;
    d.integrationScheme = IntegrationScheme::SPECIFICSCHEME;
    d.typicalCodeBlocks = {BehaviourData::FlowRule};
    d.supportedModellingHypotheses = {
        ModellingHypothesis::AXISYMMETRICALGENERALISEDPLANESTRAIN,
        ModellingHypothesis::AXISYMMETRICAL, ModellingHypothesis::PLANESTRAIN,
        ModellingHypothesis::GENERALISEDPLANESTRAIN,
        ModellingHypothesis::TRIDIMENSIONAL};
    d.supportedBehaviourSymmetries = {mfront::ISOTROPIC};
    d.allowUserDefinedStateVariables = false;
    d.allowUserDefinedIntegrationVariables = false;
    d.allowCrystalStructureDefinition = false;
    d.allowStiffnessTensorDefinition = false;
    d.minimalMFrontFileBody = "@FlowRule{}\n\n";
    return d;
  }  // end of getBehaviourDSLDescription

  void IsotropicBehaviourDSLBase::getSymbols(
      std::map<std::string, std::string>& symbols,
      const Hypothesis h,
      const std::string& n) {
    BehaviourDSLCommon::getSymbols(symbols, h, n);
    const auto& d = this->mb.getBehaviourData(h);
    for (const auto& v : d.getIntegrationVariables()) {
      getIncrementSymbol(symbols, v);
    }
    for (const auto& mv : this->mb.getMainVariables()) {
      if (Gradient::isIncrementKnown(mv.first)) {
        getIncrementSymbol(symbols, mv.first);
      }
    }
    mfront::getIncrementSymbols(symbols, d.getExternalStateVariables());
    mfront::addSymbol(symbols, "\u0394t", "dt");
  }  // end of getSymbols

  double IsotropicBehaviourDSLBase::getDefaultThetaValue() const { return 0.5; }

  void IsotropicBehaviourDSLBase::treatTheta() {
    constexpr auto h = ModellingHypothesis::UNDEFINEDHYPOTHESIS;
    this->checkNotEndOfFile("IsotropicBehaviourDSLBase::treatTheta",
                            "Cannot read theta value.");
    const auto v = tfel::utilities::convert<double>(this->current->value);
    if ((v < 0.) || (v > 1.)) {
      this->throwRuntimeError(
          "IsotropicBehaviourDSLBase::treatTheta",
          "Theta value must be positive and smaller than 1.");
    }
    ++(this->current);
    this->readSpecifiedToken("IsotropicBehaviourDSLBase::treatTheta", ";");
    this->mb.addParameter(
        h, VariableDescription("real", "\u03B8", "theta", 1u, 0u),
        BehaviourData::ALREADYREGISTRED);
    this->mb.setParameterDefaultValue(h, "theta", v);
  }  // end of treatTheta

  void IsotropicBehaviourDSLBase::treatEpsilon() {
    const auto h = ModellingHypothesis::UNDEFINEDHYPOTHESIS;
    const auto safe = this->readSafeOptionTypeIfPresent();
    this->checkNotEndOfFile("IsotropicBehaviourDSLBase::treatEpsilon",
                            "cannot read epsilon value");
    const auto epsilon = tfel::utilities::convert<double>(this->current->value);
    if (epsilon < 0) {
      this->throwRuntimeError("IsotropicBehaviourDSLBase::treatEpsilon",
                              "Epsilon value must be positive");
    }
    if ((!safe) && (epsilon < 1e-18)) {
      auto converter = std::ostringstream{};
      converter << std::scientific << epsilon;
      reportWarning(
          "the choosen default value for the convergence threshold could be "
          "too stringent (" +
          converter.str() +
          "). You may want to consider a more stringent value (1e-14 "
          "is generally a good choice).");
    }
    if ((!safe) && (epsilon > 1e-12)) {
      auto converter = std::ostringstream{};
      converter << std::scientific << epsilon;
      reportWarning(
          "the choosen default value for the convergence threshold could be "
          "too loose (" +
          converter.str() +
          "). You may want to consider a more stringent value (1e-14 "
          "is generally a good choice).");
    }
    ++(this->current);
    this->readSpecifiedToken("IsotropicBehaviourDSLBase::treatEpsilon", ";");
    this->mb.addParameter(
        h, VariableDescription("real", "\u03B5", "epsilon", 1u, 0u),
        BehaviourData::ALREADYREGISTRED);
    this->mb.setParameterDefaultValue(h, "epsilon", epsilon);
  }  // IsotropicBehaviourDSLBase::treatEpsilon

  void IsotropicBehaviourDSLBase::treatIterMax() {
    const auto h = ModellingHypothesis::UNDEFINEDHYPOTHESIS;
    const auto iterMax =
        this->readUnsignedShort("IsotropicBehaviourDSLBase::treatIterMax");
    if (iterMax == 0) {
      this->throwRuntimeError("IsotropicBehaviourDSLBase::treatIterMax",
                              "invalid value for parameter 'iterMax'");
    }
    this->readSpecifiedToken("IsotropicBehaviourDSLBase::treatIterMax", ";");
    this->mb.addParameter(h, VariableDescription("ushort", "iterMax", 1u, 0u),
                          BehaviourData::ALREADYREGISTRED);
    this->mb.setParameterDefaultValue(h, "iterMax", iterMax);
  }  // end of treatIterMax

  std::string IsotropicBehaviourDSLBase::flowRuleVariableModifier(
      const Hypothesis h, const std::string& var, const bool addThisPtr) {
    const auto& d = this->mb.getBehaviourData(h);
    if ((d.isExternalStateVariableName(var)) ||
        (d.isStateVariableName(var))) {
      if (addThisPtr) {
        return "this->" + var + "_";
      } else {
        return var + "_";
      }
    }
    if (d.isAuxiliaryStateVariableName(var)){
      const auto& v = d.getAuxiliaryStateVariables().getVariable(var);
      if (v.getAttribute<bool>("ComputedByExternalModel", false)) {
        if (addThisPtr) {
          return "(this->" + var + " + (this->theta) * (this->d" + var + "))";
        } else {
          return "(" + var + " + (this->theta) * d" + var + ")";
        }
      }
    }
    if ((d.isExternalStateVariableIncrementName(var)) ||
        (var == "dT")) {
      this->declareExternalStateVariableProbablyUnusableInPurelyImplicitResolution(
          h, var.substr(1));
    }
    if (addThisPtr) {
      return "this->" + var;
    }
    return var;
  }  // end of flowRuleVariableModifier

  void IsotropicBehaviourDSLBase::checkFlowRule(std::string_view n) const {
    auto warnings = std::vector<std::string>{};
    for (const auto h : this->mb.getDistinctModellingHypotheses()) {
      auto report = [&warnings, h](const std::string& msg) {
        warnings.push_back(
            msg + ". This warning can be disabled by using the <safe> option.");
      };
      auto report_unexpected = [&report, &n](std::string_view v) {
        report("using " + std::string{v} + " in the body of the '" +
               std::string{n} +
               "' code block is unexpected and can be a mistake");
      };
      const auto& d = this->mb.getBehaviourData(h);
      const auto& c = d.getCodeBlock(std::string{n});
      if (isSafe(c)) {
        continue;
      }
      for (const auto& m : c.members) {
        if (m == "theta") {
          report_unexpected("the 'theta' parameter");
        }
        if (m == "iterMax") {
          report_unexpected("the 'iterMax' parameter");
        }
        if (m == "dt") {
          report_unexpected("the time increment 'dt'");
        }
        if (d.isAuxiliaryStateVariableName(m)) {
          const auto& v = d.getAuxiliaryStateVariables().getVariable(m);
          if (!v.getAttribute<bool>("ComputedByExternalModel", false)) {
            report_unexpected("variable '" + m + "'");
          }
        }
        if ((this->mb.isGradientName(m)) ||
            (this->mb.isGradientIncrementName(m)) ||
            (this->mb.isThermodynamicForceName(m)) ||
            (d.isIntegrationVariableIncrementName(m))) {
          report_unexpected("variable '" + m + "'");
        }
      }
    }
    reportWarning(warnings);
  }

  void IsotropicBehaviourDSLBase::treatFlowRule() {
    std::function<std::string(const Hypothesis, const std::string&, const bool)>
        modifier =
            [this](const Hypothesis h, const std::string& sv, const bool b) {
              return this->flowRuleVariableModifier(h, sv, b);
            };
    this->treatCodeBlock(BehaviourData::FlowRule, modifier, true, false);
    this->checkFlowRule(BehaviourData::FlowRule);
  }  // end of treatFlowRule

  void IsotropicBehaviourDSLBase::treatExternalStateVariable() {
    VariableDescriptionContainer ev;
    std::set<Hypothesis> h;
    this->readVariableList(
        ev, h, &BehaviourDescription::addExternalStateVariables, true);
    for (const auto& elem : h) {
      CodeBlock ib;
      for (const auto& v : ev) {
        const auto currentVarName = v.name + "_";
        this->mb.addLocalVariable(
            elem, VariableDescription(v.type, currentVarName, v.arraySize, 0u));
        ib.code = "this->" + currentVarName + " = this->" + v.name + "+(" +
                  this->mb.getClassName() + "::theta)*(this->d" + v.name +
                  ");\n";
      }
      this->mb.setCode(elem, BehaviourData::BeforeInitializeLocalVariables, ib,
                       BehaviourData::CREATEORAPPEND, BehaviourData::AT_END);
    }
  }  // end of treatExternalStateVariable

  void IsotropicBehaviourDSLBase::completeVariableDeclaration() {
    using namespace tfel::glossary;
    auto add_lv = [](BehaviourDescription& bd, const std::string& t,
                     const std::string& sn, const std::string& n,
                     const std::string& g, const std::string d) {
      const auto h = ModellingHypothesis::UNDEFINEDHYPOTHESIS;
      auto r = bd.checkVariableExistence(n, "Parameter", false);
      if (!r.first) {
        auto v = (!sn.empty()) ? VariableDescription(t, sn, n, 1u, 0u)
                               : VariableDescription(t, n, 1u, 0u);
        v.description = d;
        bd.addLocalVariable(h, v, BehaviourData::UNREGISTRED);
      } else {
        tfel::raise_if(!r.second,
                       "ImplicitDSLBase::completeVariableDeclaration: "
                       "Parameter '" +
                           n + "' is not defined for all hypotheses");
        if (!g.empty()) {
          bd.checkVariableGlossaryName(n, g);
        }
      }
    };
    const auto h = ModellingHypothesis::UNDEFINEDHYPOTHESIS;
    if (getVerboseMode() >= VERBOSE_DEBUG) {
      auto& log = getLogStream();
      log << "IsotropicBehaviourDSLBase::completeVariableDeclaration : begin\n";
    }
    BehaviourDSLCommon::completeVariableDeclaration();
    // intermediate temperature
    const auto* const Topt = BehaviourDescription::
        automaticDeclarationOfTheTemperatureAsFirstExternalStateVariable;
    if (this->mb.getAttribute<bool>(Topt)) {
      CodeBlock initLocalVars;
      initLocalVars.code = "this->T_ = this->T + (" + this->getClassName() +
                           "::theta) * (this->dT);\n";
      this->mb.setCode(ModellingHypothesis::UNDEFINEDHYPOTHESIS,
                       BehaviourData::BeforeInitializeLocalVariables,
                       initLocalVars, BehaviourData::CREATEORAPPEND,
                       BehaviourData::BODY);
    }
    add_lv(this->mb, "stress", "\u03BB", "lambda",
           Glossary::FirstLameCoefficient,
           "first Lamé coefficient at t+theta*dt");
    add_lv(this->mb, "stress", "\u03BC", "mu", Glossary::ShearModulus,
           "shear modulus at t+theta*dt");
    add_lv(this->mb, "stress", "\u03BB\u2091\u209C\u209B", "lambda_tdt", "",
           "first Lamé coefficient at t+dt");
    add_lv(this->mb, "stress", "\u03BC\u2091\u209C\u209B", "mu_tdt", "",
           "shear modulus at t+dt");
    if (this->mb.areElasticMaterialPropertiesDefined()) {
      add_lv(this->mb, "stress", "", "young", Glossary::YoungModulus,
             "Young modulus at t+theta*dt");
      add_lv(this->mb, "stress", "", "young_tdt", "", "Young modulus at t+dt");
      add_lv(this->mb, "real", "\u03BD", "nu", Glossary::PoissonRatio,
             "Poisson ratio at t+theta*dt");
      add_lv(this->mb, "real", "\u03BD\u2091\u209C\u209B", "nu_tdt", "",
             "Poisson ratio at t+dt");
    } else {
      this->mb.addMaterialProperty(
          h, VariableDescription("stress", "young", 1u, 0u));
      this->mb.setGlossaryName(h, "young", "YoungModulus");
      this->mb.addMaterialProperty(
          h, VariableDescription("real", "\u03BD", "nu", 1u, 0u));
      this->mb.setGlossaryName(h, "nu", "PoissonRatio");
    }
    if (!this->mb.hasParameter(h, "theta")) {
      this->mb.addParameter(
          h, VariableDescription("real", "\u03B8", "theta", 1u, 0u),
          BehaviourData::ALREADYREGISTRED);
      this->mb.setParameterDefaultValue(h, "theta",
                                        this->getDefaultThetaValue());
    }
    if (!this->mb.hasParameter(h, "epsilon")) {
      this->mb.addParameter(
          h, VariableDescription("real", "\u03B5", "epsilon", 1u, 0u),
          BehaviourData::ALREADYREGISTRED);
      this->mb.setParameterDefaultValue(h, "epsilon", 1.e-8);
      reportWarning(
          "using the default value for the convergence threshold. "
          "This value is generally considered too loose. You may "
          "want to consider a more stringent value (1e-14 is a good choice). "
          "See the `@Epsilon` keyword for details");
    }
    if (!this->mb.hasParameter(h, "iterMax")) {
      unsigned short iterMax = 100u;
      this->mb.addParameter(h, VariableDescription("ushort", "iterMax", 1u, 0u),
                            BehaviourData::ALREADYREGISTRED);
      this->mb.setParameterDefaultValue(h, "iterMax", iterMax);
    }
    if (getVerboseMode() >= VERBOSE_DEBUG) {
      auto& log = getLogStream();
      log << "IsotropicBehaviourDSLBase::completeVariableDeclaration: end\n";
    }
  }  // end of completeVariableDeclaration

  void IsotropicBehaviourDSLBase::endsInputFileProcessing() {
    if (getVerboseMode() >= VERBOSE_DEBUG) {
      auto& log = getLogStream();
      log << "IsotropicBehaviourDSLBase::endsInputFileProcessing: begin\n";
    }
    BehaviourDSLCommon::endsInputFileProcessing();
    if (getVerboseMode() >= VERBOSE_DEBUG) {
      auto& log = getLogStream();
      log << "IsotropicBehaviourDSLBase::endsInputFileProcessing: end\n";
    }
  }  // end of endsInputFileProcessing

  IsotropicBehaviourDSLBase::~IsotropicBehaviourDSLBase() = default;

}  // end of namespace mfront
