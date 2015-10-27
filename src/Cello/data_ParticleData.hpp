// See LICENSE_CELLO file for license and copyright information

/// @file     data_ParticleData.hpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     2015-10-12
/// @brief    [\ref Data] Declaration of the ParticleData class

#ifndef DATA_PARTICLE_DATA_HPP
#define DATA_PARTICLE_DATA_HPP

class ParticleData {

  /// @class    ParticleData
  /// @ingroup  Data
  /// @brief    [\ref Data] 

  friend class Particle;

public: // interface

  /// Constructor
  ParticleData();

  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p);

  /// Return the attribute array for the given particle type and batch

  char * attribute_array (ParticleDescr *, int it, int ib, int ia);

  /// Return the number of batches of particles for the given type.

  int num_batches (int it) const;

  /// Return the number of particles in the given batch, of the given
  /// type, or total on the block.

  int num_particles (ParticleDescr *) const;
  int num_particles (ParticleDescr *, int it) const;
  int num_particles (ParticleDescr *, int it, int ib) const;

  /// Create the given number of particles of the given type.  Always
  /// creates them at the end instead of filling up any unused
  /// particle spaces in earlier batches, to ease initialization via
  /// index()

  int insert_particles (ParticleDescr *, int it, int np);

  /// Delete the given particles in the batch according to mask
  /// attribute.  Compresses the batch if particles deleted, so batch
  /// may have fewer than max number of particles.  Other batches
  /// remain unchanged.

  void delete_particles (ParticleDescr *, int it, int ib, const bool * m);

  /// Same as delete, but inserts particles into a second Particle
  /// object.

  void split_particles (ParticleDescr *, int it, int ib, const bool *m,
			ParticleData * particle_data);

  /// Compress particles in batches so that ib'th batch size equals
  /// batch_size.  May be performed periodically to recover space lost
  /// in multiple insert/deletes

  void compress (ParticleDescr *, int it, int ib, const bool * m);

  /// Add a new type to the attribute_array.  Should only be called
  /// by Particle.

  void new_type()
  {
    attribute_array_.resize(attribute_array_.size()+1);
    attribute_align_.resize(attribute_align_.size()+1);
    particle_count_.resize(particle_count_.size()+1);
  }

private: /// functions

  /// Return an id (not "index"); for a particle that is guaranteed to
  /// be unique across all processors.  May involve communication.

  /// long long assign_id_ ()

  /// Allocate arrays based on ParticleDescr attributes
  void allocate_(ParticleDescr * particle_descr)
  {
    const size_t nt = particle_descr->num_types();
    if (attribute_array_.size() < nt) {
      attribute_array_.resize(nt);
    }
    if (attribute_align_.size() < nt) {
      attribute_align_.resize(nt);
    }
    if (particle_count_.size() < nt) {
      particle_count_.resize(nt);
    }
  };

  /// Allocate attribute_array_ block, aligned at 16 byte boundary
  /// with updated attribute_align_
  void resize_array_ (ParticleDescr *, int it, int ib, int np);

  void check_arrays_ (ParticleDescr * particle_descr,
		      std::string file, int line) const;

private: /// attributes

  /// Array of blocks of particle attributes array_[it][ib];
  std::vector< std::vector< std::vector<char> > > attribute_array_;

  /// Alignment adjustment to correct for 16-byte alignment of
  /// first attribute in each batch

  std::vector< std::vector< char > > attribute_align_;

  /// Number of particles in the batch particle_count_[it][ib];
  std::vector < std::vector < int > > particle_count_;

};

#endif /* DATA_PARTICLE_DATA_HPP */

