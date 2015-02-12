// See LICENSE_CELLO file for license and copyright information

/// @file     mesh_Data.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2013-03-10
/// @brief    [\ref Mesh] Declaration of the Data class
///

#ifndef MESH_DATA_HPP
#define MESH_DATA_HPP

class Data {

  friend class Block;
  friend class IoBlock;

  /// @class    Data
  /// @ingroup  Mesh
  /// @brief    [\ref Mesh] 

public: // interface

  /// Constructor
  Data(FieldDescr * field_descr,
	int nx, int ny, int nz,
	int num_field_blocks,
	double xm, double xp,
	double ym, double yp,
	double zm, double zp) throw();

  /// Destructor
  virtual ~Data() throw();

  /// Copy constructor
  Data(const Data & data) throw();

  /// Assignment operator
  Data & operator= (const Data & data) throw();

  /// Empty constructor
  Data() { }

  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {
    TRACEPUP;
    bool up = p.isUnpacking();
    p | num_field_blocks_;
    // allocate field_block_[] vector first if unpacking
    if (up) field_block_.resize(num_field_blocks_);
    for (int i=0; i<num_field_blocks_; i++) {
      if (up) field_block_[i] = new FieldBlock;
      p | *field_block_[i];
    }
    PUParray(p,lower_,3);
    PUParray(p,upper_,3);
    // NOTE: change this function whenever attributes change
  }

  //----------------------------------------------------------------------

  /// Return the number of FieldBlocks
  int num_field_blocks() const throw()
  { return num_field_blocks_; }

  /// Return the ith Field block
  const FieldBlock * field_block (int i=0) const throw()
  { return field_block_.at(i); }

  /// Return the ith Field block
  FieldBlock * field_block (int i=0) throw()
  { return field_block_.at(i); }

  /// Return the ith Field descriptor
  const FieldDescr * field_descr (int i=0) const throw()
  { return field_block_.at(i)->field_descr(); }

  /// Return the ith Field
  Field field (int i=0) throw()
  { return Field((FieldDescr *)(field_block_.at(i)->field_descr()),
		 field_block_.at(i)); }

  /// Return the x,y,z,t coordinates of field cell centers
  void field_cells (double * x, double * y, double * z,
		    int gx = 0, int gy = 0, int gz = 0) const;

  /// Return domain lower extent
  /// B
  inline void lower(double * x, 
		    double * y = 0,
		    double * z = 0) const throw ()
  {
    if (x) *x = lower_[0];
    if (y) *y = lower_[1];
    if (z) *z = lower_[2];
  }

  //----------------------------------------------------------------------

  /// Return domain upper extent
  /// B
  inline void upper(double * x,
		    double * y = 0,
		    double * z = 0) const throw ()
  {
    if (x) *x = upper_[0];
    if (y) *y = upper_[1];
    if (z) *z = upper_[2];
  }


public: // virtual functions

  virtual void allocate (const FieldDescr * field_descr) throw();

private: // functions

  void copy_(const Data & data) throw();

private: // attributes

  /// Number of field blocks (required by CHARM++ PUP::er)
  int num_field_blocks_;

  /// Array of field blocks
  std::vector<FieldBlock *> field_block_;

  /// Lower extent of the box associated with the block [computable]
  double lower_[3];

  /// Upper extent of the box associated with the block [computable]
  double upper_[3];

  // NOTE: change pup() function whenever attributes change

};

#endif /* MESH_DATA_HPP */

