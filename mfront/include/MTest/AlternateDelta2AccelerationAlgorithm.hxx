/*! 
 * \file  mfront/include/MTest/AlternateDelta2AccelerationAlgorithm.hxx
 * \brief
 * \author RAMIERE Isabelle
 * \brief 16 mars 2015
 * \copyright Copyright (C) 2006-2014 CEA/DEN, EDF R&D. All rights 
 * reserved. 
 * This project is publicly released under either the GNU GPL Licence 
 * or the CECILL-A licence. A copy of thoses licences are delivered 
 * with the sources of TFEL. CEA or EDF may also distribute this 
 * project under specific licensing conditions. 
 */

#ifndef LIB_MFRONT_MTEST_MTESTALTERNATEDELTA2ACCELERATIONALGORITHM_H_
#define LIB_MFRONT_MTEST_MTESTALTERNATEDELTA2ACCELERATIONALGORITHM_H_ 

#include"MTest/MTestConfig.hxx"
#include"MTest/AccelerationAlgorithm.hxx"

namespace mfront
{

  /*!
   * \brief This class implements the secant acceleration algorithm.
   */
  struct MFRONT_MTEST_VISIBILITY_EXPORT AlternateDelta2AccelerationAlgorithm
    : public AccelerationAlgorithm
  {
    //! default constructor
    AlternateDelta2AccelerationAlgorithm();
    /*!
     * \return the name of the acceleration algorithm
     */
    virtual std::string
    getName(void) const override;
    /*!
     * \param[in] psz : total number of unknowns (problem size)
     */
    virtual void
    initialize(const unsigned short) override;
    /*!
     * set a parameter of the acceleration algorithm
     * \param[in] p : parameter name
     * \param[in] v : parameter value
     */
    virtual void
    setParameter(const std::string&,
		 const std::string&) override;
    /*!
     * called at each time step, before the beginning of the Newton
     * algorithm
     */
    virtual void
    preExecuteTasks(void) override;
    /*!
     * \param[in,out] u1   : current estimate of the unknowns
     * \param[in]     ru   : fixed point residual on the unknowns
     * \param[in]     rf   : Newton residual
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
	    const unsigned short) override;
    /*!
     * called at each time step, once convergence is reached
     */
    virtual void
    postExecuteTasks(void) override;
    //! destructor
    virtual ~AlternateDelta2AccelerationAlgorithm();
  protected:
    tfel::math::vector<real> asa_u0;
    tfel::math::vector<real> asa_u1;
    tfel::math::vector<real> asa_du0;
    tfel::math::vector<real> asa_du1;
    tfel::math::vector<real> asa_d2u;
    tfel::math::vector<real> asa_r0;
    tfel::math::vector<real> asa_r1;
    tfel::math::vector<real> asa_dr0;
    tfel::math::vector<real> asa_dr1;
    tfel::math::vector<real> asa_d2r;
    //! trigger
    int asat;
  }; // end of struct AlternateDelta2AccelerationAlgorithm

} // end of namespace mfront

#endif /* LIB_MFRONT_MTEST_MTESTALTERNATEDELTA2ACCELERATIONALGORITHM_H_ */
