/*!
 * \file   StressPotential.hxx
 * \brief
 * \author Thomas Helfer
 * \date   20/03/2018
 * \copyright Copyright (C) 2006-2018 CEA/DEN, EDF R&D. All rights
 * reserved.
 * This project is publicly released under either the GNU GPL Licence
 * or the CECILL-A licence. A copy of thoses licences are delivered
 * with the sources of TFEL. CEA or EDF may also distribute this
 * project under specific licensing conditions.
 * <!-- Local IspellDict: english -->
 */

#ifndef LIB_MFRONT_BEHAVIOURBRICK_STRESSPOTENTIAL_HXX
#define LIB_MFRONT_BEHAVIOURBRICK_STRESSPOTENTIAL_HXX

#include <map>
#include <string>
#include <vector>
#include "TFEL/Material/ModellingHypothesis.hxx"
#include "MFront/VariableDescription.hxx"
#include "MFront/BehaviourDescription.hxx"

namespace tfel {
  namespace utilities {
    // forward declaration
    struct Data;
  }  // end of namespace utilities
}  // end of namespace tfel

namespace mfront {

  // forward declaration
  struct AbstractBehaviourDSL;

  namespace bbrick {

    // forward declaration
    struct OptionDescription;

    /*!
     * \brief class describing the computation of the stress.
     */
    struct StressPotential {
      //! a simple alias
      using DataMap = std::map<std::string, tfel::utilities::Data>;
      //! a simple alias
      using ModellingHypothesis = tfel::material::ModellingHypothesis;
      //! a simple alias
      using Hypothesis = ModellingHypothesis::Hypothesis;
      //! a simple alias
      using MaterialProperty = BehaviourDescription::MaterialProperty;
      //! \return the name of the stress potential
      virtual std::string getName() const = 0;
      //! \return the stress potential option description
      virtual std::vector<OptionDescription> getOptions() const = 0;
      /*!
       * \param[in,out] bd: behaviour description
       * \param[in,out] dsl: abstract behaviour dsl
       * \param[in] d: options
       */
      virtual void initialize(BehaviourDescription&,
                              AbstractBehaviourDSL&,
                              const DataMap&) = 0;
      /*!
       * \brief This method returns the list of supported modelling
       * hypotheses that are supported by the stress potential.
       *
       * This method is called after the input file processing. Two
       * cases may arise:
       * - if the user has specified a list of supported modelling
       *   hypotheses, this list is used to check if the user's request
       *   can be fulfilled. If the stress potential returns an empty list, it
       *   means that the stress potential does not have any restriction.
       * - if the user has not specified the list of supported modelling
       *   hypotheses, this method is used to select a list of supported
       *   modelling hypotheses. If the stress potential returns an empty list,
       *   it means that the stress potential does not participate to the
       *   modelling hypotheses selection. If all the stress potentials returns
       *   an empty list, a set of default modelling hypotheses is selected.
       *
       * \param[in/out] bd: behaviour description
       * \param[in] dsl: abstract behaviour dsl
       */
      virtual std::vector<Hypothesis> getSupportedModellingHypotheses(
          const BehaviourDescription&, const AbstractBehaviourDSL&) const = 0;
      /*!
       * \brief complete the variable description
       * \param[in] dsl: abstract behaviour dsl

       */
      virtual void completeVariableDeclaration(
          BehaviourDescription&, const AbstractBehaviourDSL&) const = 0;
      /*!
       * \brief method called at the end of the input file processing
       * \param[in/out] bd: behaviour description
       * \param[in] dsl: abstract behaviour dsl
       */
      virtual void endTreatment(BehaviourDescription&,
                                const AbstractBehaviourDSL&) const = 0;
      /*!
       * \return the list of integration variables defined by the potential for
       * which a derivative of the stress with respect to this variable exists.
       */
      virtual VariableDescriptionContainer getStressVariables() const = 0;
      /*!
       * \brief add the definition of the stress derivatives with respect to
       * the integration variables used to define it. This definition is added
       * to the `BehaviourData::Integrator` code block.
       * \param[in/out] bd: behaviour description
       */
      virtual void writeStressDerivatives(BehaviourDescription&) const = 0;
      //! destructor
      virtual ~StressPotential();
    };  // end of struct StressPotential

  }  // end of namespace bbrick

}  // end of namespace mfront

#endif /* LIB_MFRONT_BEHAVIOURBRICK_STRESSPOTENTIAL_HXX */