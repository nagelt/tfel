/*!
 * \file   include/TFEL/Math/Function/Id.hxx
 * \brief  This file declares the Id class
 * \author Helfer Thomas
 * \date   09 Jan 2007
 * \copyright Copyright (C) 2006-2014 CEA/DEN, EDF R&D. All rights 
 * reserved. 
 * This project is publicly released under either the GNU GPL Licence 
 * or the CECILL-A licence. A copy of thoses licences are delivered 
 * with the sources of TFEL. CEA or EDF may also distribute this 
 * project under specific licensing conditions. 
 */

#ifndef _LIB_TFEL_ID_H_
#define _LIB_TFEL_ID_H_ 

#include"TFEL/Config/TFELConfig.hxx"
#include"TFEL/Metaprogramming/EnableIf.hxx"
#include"TFEL/TypeTraits/IsScalar.hxx"
#include"TFEL/Math/General/UnaryResultType.hxx"
#include"TFEL/Math/General/ComputeUnaryResult.hxx"
#include"TFEL/Math/Function/FunctionConcept.hxx"

namespace tfel{

  namespace math{

    // forward declaration (see Cst.hxx)
    template<short N,unsigned short D>
    struct Cst;

    struct FctId;

    template<>
    struct FunctionTraits<FctId>
    {
      typedef Cst<1,1> DerivateFunc;
    };

    struct TFELMATH_VISIBILITY_EXPORT FctId
      : public FunctionConcept<FctId>
    {
    
      /*!
       * \brief  Return the name of the class.
       * \param  void.
       * \return std::string, the name of the class.
       * \see    Name.
       */
      static std::string getName(void);

      template<typename T>
      typename tfel::meta::EnableIf<                                                      
	tfel::typetraits::IsScalar<T>::cond,
	T                               
	>::type                                                                             
      operator()(const T s) const
      {
	return s;
      }
  
      template<typename T>      			                                  
      typename tfel::meta::EnableIf<                                                      
        !tfel::typetraits::IsInvalid<typename ComputeUnaryResult<T,FctId>::Result>::cond&&
	!tfel::typetraits::IsScalar<T>::cond,
        const typename ComputeUnaryResult<T,FctId>::Handle                               
      >::type                                                                             
      operator()(const T& v) const
      {
	typedef typename ComputeUnaryResult<T,FctId>::Handle Handle;                     
	return Handle(v,*this);                                                           
      }
    
    }; // end of FctId

    /*
     * Partial Specialisation for scalars
     */
    template<typename T>
    struct UnaryResultType_<ScalarTag,FunctionTag,T,FctId>
    {
      typedef T type;
    };

    TFELMATH_VISIBILITY_EXPORT
    Cst<1,1>
    derivate(const FctId);

    namespace stdfunctions{
      extern TFELMATH_VISIBILITY_EXPORT tfel::math::FctId id TFEL_UNUSED_ATTRIBUTE;
    }

  } // end of namespace math

} // end of namespace tfel

#endif /* _LIB_TFEL_ID_H */
