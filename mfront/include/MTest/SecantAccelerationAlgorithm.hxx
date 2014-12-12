/*! 
 * \file  mfront/include/MTest/SecantAccelerationAlgorithm.hxx
 * \brief
 * \author Helfer Thomas
 * \brief 10 juin 2014
 * \copyright Copyright (C) 2006-2014 CEA/DEN, EDF R&D. All rights 
 * reserved. 
 * This project is publicly released under either the GNU GPL Licence 
 * or the CECILL-A licence. A copy of thoses licences are delivered 
 * with the sources of TFEL. CEA or EDF may also distribute this 
 * project under specific licensing conditions. 
 */

#ifndef _LIB_MFRONT_MTEST_MTESTSECANTACCELERATIONALGORITHM_H_
#define _LIB_MFRONT_MTEST_MTESTSECANTACCELERATIONALGORITHM_H_ 

#include"MTest/MTestConfig.hxx"
#include"MTest/AccelerationAlgorithm.hxx"

namespace mfront
{

  /*!
   * \brief This class implements the secant acceleration algorithm.
   */
  struct MFRONT_MTEST_VISIBILITY_EXPORT SecantAccelerationAlgorithm
    : public AccelerationAlgorithm
  {
    //! default constructor
    SecantAccelerationAlgorithm();
    /*!
     * \return the name of the acceleration algorithm
     */
    virtual std::string
    getName(void) const;
    /*!
     * \param[in] psz : total number of unknowns (problem size)
     */
    virtual void
    initialize(const unsigned short);
    /*!
     * set a parameter of the acceleration algorithm
     * \param[in] p : parameter name
     * \param[in] v : parameter value
     */
    virtual void
    setParameter(const std::string&,
		 const std::string&);
    /*!
     * called at each time step, before the beginning of the Newton
     * algorithm
     */
    virtual void
    preExecuteTasks(void);
    /*!
     * \param[in,out] u1   : current estimate of the unknowns
     * \param[in]     u0   : previous estimate of the unknowns
     * \param[in]     r    : Newton residual
     * \param[in]     ueps : criterium on the unknowns
     * \param[in]     seps : criterium on the thermodynamic forces
     * \param[in]     iter : current iteration number
     */
    virtual void
    execute(tfel::math::vector<real>&,
	    const tfel::math::vector<real>&,
	    const tfel::math::vector<real>&,
	    const real,
	    const real,
	    const unsigned short);
    /*!
     * called at each time step, once convergence is reached
     */
    virtual void
    postExecuteTasks(void);
    //! destructor
    virtual ~SecantAccelerationAlgorithm();
  protected:
    // secant acceleration algorithm
    tfel::math::vector<real> sa_u0;
    tfel::math::vector<real> sa_u1;
    tfel::math::vector<real> sa_r0;
    tfel::math::vector<real> sa_r1;
    tfel::math::vector<real> sa_r;
    tfel::math::vector<real> sa_dr;
    /*!
     * parameter of the secant algorithm
     */
    real sa_w;
    //! trigger
    int sat;
  }; // end of struct SecantAccelerationAlgorithm

} // end of namespace mfront

#endif /* _LIB_MFRONT_MTEST_MTESTSECANTACCELERATIONALGORITHM_H */
