#ifndef __assembly_h__
#define __assembly_h__

// rbOOmit includes
#include "rb_parameters.h"
#include "rb_theta.h"
#include "rb_theta_expansion.h"
#include "rb_assembly_expansion.h"

// boundary IDs
#define BOUNDARY_ID_MIN_Z 0
#define BOUNDARY_ID_MIN_Y 1
#define BOUNDARY_ID_MAX_X 2
#define BOUNDARY_ID_MAX_Y 3
#define BOUNDARY_ID_MIN_X 4
#define BOUNDARY_ID_MAX_Z 5

class ElasticityRBConstruction;

struct ElasticityAssembly : ElemAssembly
{

	ElasticityAssembly(ElasticityRBConstruction& rb_sys_in)
	: 
  rb_sys(rb_sys_in)
  {}
  
	// Kronecker delta function
	inline Real kronecker_delta(unsigned int i,
                              unsigned int j)
	{
		return i == j ? 1. : 0.;
	}
	
	Real elasticity_tensor(unsigned int i,
                         unsigned int j,
                         unsigned int k,
                         unsigned int l)
  {
		// Define the Poisson ratio and Young's modulus
		const Real nu = 0.3;
		const Real E  = 1.;

		// Define the Lame constants (lambda_1 and lambda_2) based on nu and E
		const Real lambda_1 = E * nu / ( (1. + nu) * (1. - 2.*nu) );
		const Real lambda_2 = 0.5 * E / (1. + nu);

		return lambda_1 * kronecker_delta(i,j) * kronecker_delta(k,l)
		     + lambda_2 * (kronecker_delta(i,k) * kronecker_delta(j,l) + kronecker_delta(i,l) * kronecker_delta(j,k));
	}
	
	/**
	 * The ElasticityRBConstruction object that will use this assembly.
	 */
	ElasticityRBConstruction& rb_sys;
};

struct ThetaA0 : RBTheta { virtual Number evaluate(const RBParameters& ) { return 1.; } };
struct AssemblyA0 : ElasticityAssembly
{

  AssemblyA0(ElasticityRBConstruction& rb_sys_in)
  :
  ElasticityAssembly(rb_sys_in)
  {}
  
  // The interior assembly operator
	virtual void interior_assembly(FEMContext &c);

};

struct ThetaA1 : RBTheta { virtual Number evaluate(const RBParameters& mu) { return mu.get_value("x_scaling"); } };
struct AssemblyA1 : ElasticityAssembly
{

  AssemblyA1(ElasticityRBConstruction& rb_sys_in)
  :
  ElasticityAssembly(rb_sys_in)
  {}
  
  // The interior assembly operator
	virtual void interior_assembly(FEMContext &c);

};

struct ThetaA2 : RBTheta { virtual Number evaluate(const RBParameters& mu) { return 1./mu.get_value("x_scaling"); } };
struct AssemblyA2 : ElasticityAssembly
{

  AssemblyA2(ElasticityRBConstruction& rb_sys_in)
  :
  ElasticityAssembly(rb_sys_in)
  {}
  
  // The interior assembly operator
	virtual void interior_assembly(FEMContext &c);

};

struct ThetaF0 : RBTheta { virtual Number evaluate(const RBParameters& mu) { return mu.get_value("load_Fx"); } };
struct AssemblyF0 : ElasticityAssembly
{
  AssemblyF0(ElasticityRBConstruction& rb_sys_in)
  :
  ElasticityAssembly(rb_sys_in)
  {}

  // Apply a traction 
  virtual void boundary_assembly(FEMContext &c);

};

struct ThetaF1 : RBTheta { virtual Number evaluate(const RBParameters& mu)   { return mu.get_value("load_Fy"); } };
struct AssemblyF1 : ElasticityAssembly
{
  AssemblyF1(ElasticityRBConstruction& rb_sys_in)
  :
  ElasticityAssembly(rb_sys_in)
  {}
  
  // Apply a traction 
  virtual void boundary_assembly(FEMContext &c);
};

struct ThetaF2 : RBTheta { virtual Number evaluate(const RBParameters& mu)   { return mu.get_value("load_Fz"); } };
struct AssemblyF2 : ElasticityAssembly
{
  AssemblyF2(ElasticityRBConstruction& rb_sys_in)
  :
  ElasticityAssembly(rb_sys_in)
  {}
  
  // Apply a traction 
  virtual void boundary_assembly(FEMContext &c);
};

struct InnerProductAssembly : ElasticityAssembly
{

  InnerProductAssembly(ElasticityRBConstruction& rb_sys_in)
  :
  ElasticityAssembly(rb_sys_in)
  {}
  
  // The interior assembly operator
	virtual void interior_assembly(FEMContext &c);

};

// Define an RBThetaExpansion class for this PDE
struct ElasticityThetaExpansion : RBThetaExpansion
{

  /**
   * Constructor.
   */
  ElasticityThetaExpansion()
  {
    // set up the RBThetaExpansion object
    attach_A_theta(&theta_a_0);
    attach_A_theta(&theta_a_1);
    attach_A_theta(&theta_a_2);
    attach_F_theta(&theta_f_0);
    attach_F_theta(&theta_f_1);
    attach_F_theta(&theta_f_2);
  }

  // The RBTheta member variables
  ThetaA0 theta_a_0;
  ThetaA1 theta_a_1;
  ThetaA2 theta_a_2;
  ThetaF0 theta_f_0;
  ThetaF1 theta_f_1;
  ThetaF2 theta_f_2;
};

// Define an RBAssemblyExpansion class for this PDE
struct ElasticityAssemblyExpansion : RBAssemblyExpansion
{

  /**
   * Constructor.
   */
  ElasticityAssemblyExpansion(ElasticityRBConstruction& rb_sys_in)
  :
  A0_assembly(rb_sys_in),
  A1_assembly(rb_sys_in),
  A2_assembly(rb_sys_in),
  F0_assembly(rb_sys_in),
  F1_assembly(rb_sys_in),
  F2_assembly(rb_sys_in)
  {
    // And set up the RBAssemblyExpansion object
    attach_A_assembly(&A0_assembly);
    attach_A_assembly(&A1_assembly);
    attach_A_assembly(&A2_assembly);
    attach_F_assembly(&F0_assembly);
    attach_F_assembly(&F1_assembly);
    attach_F_assembly(&F2_assembly);
  }

  // The ElemAssembly objects
  AssemblyA0 A0_assembly;
  AssemblyA1 A1_assembly;
  AssemblyA2 A2_assembly;
  AssemblyF0 F0_assembly;
  AssemblyF1 F1_assembly;
  AssemblyF2 F2_assembly;
};

#endif