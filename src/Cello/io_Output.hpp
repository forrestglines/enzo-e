// See LICENSE_CELLO file for license and copyright information

/// @file     io_Output.hpp 
/// @author   James Bordner (jobordner@ucsd.edu) 
/// @date     2011-03-14
/// @brief    [\ref Io] Declaration of the Output class

#ifndef IO_OUTPUT_HPP
#define IO_OUTPUT_HPP

class Factory;
class FieldDescr;
class Hierarchy;
class ItField;
class Patch;
class Schedule;
class Simulation;

class Output {

  /// @class    Output
  /// @ingroup  Io
  /// @brief [\ref Io] define interface for various types of IO for
  /// Simulations

public: // functions

  /// Create an uninitialized Output object
  Output(const Factory * factory) throw();

  /// Delete an Output object
  virtual ~Output() throw();

#ifdef CONFIG_USE_CHARM
  /// CHARM++ Pack / Unpack function
  inline void pup (PUP::er &p)
  {
    // NOTE: change this function whenever attributes change
    p | *file_;
    p | *schedule_;
    p | process_;
    p | loop_;
    p | index_charm_;
    p | cycle_;
    p | count_;
    p | time_;
    p | file_name_;
    p | file_args_;
    p | *it_field_;
    p | *io_block_;
    p | *io_field_block_;
    p | process_stride_;

  }
#endif

  /// Set file name
  void set_filename (std::string filename,
		     std::vector<std::string> fileargs) throw()
  { file_name_ = filename;  file_args_ = fileargs;  }

  /// Set field iterator
  void set_it_field (ItField * it_field) throw()
  { it_field_ = it_field; }
  
  /// Return the IoBlock object
  IoBlock * io_block () const throw() { return io_block_; }

  /// Return the IoFieldBlock object
  IoFieldBlock * io_field_block () const throw() { return io_field_block_; }

  /// Return the File object pointer
  File * file() throw() 
  { return file_; };

  /// Return the Schedule object pointer
  Schedule * schedule() throw() 
  { return schedule_; };

  int process_stride () const throw () 
  { return process_stride_; };

  void set_process_stride (int stride) throw () 
  {
    process_stride_ = stride; 
#ifdef CONFIG_USE_CHARM
    loop_.stop() = process_stride_;
#endif
  };

  /// Return whether output is scheduled for this cycle
  bool is_scheduled (int cycle, double time);

  /// Return whether this process is a writer
  bool is_writer () const throw () 
  { return (process_ == process_writer()); };

  /// Return the process id of the writer for this process id
  int process_writer() const throw()
  {
    return process_ - (process_ % process_stride_);
  }

  /// Return the updated timestep if time + dt goes past a scheduled output
  double update_timestep (double time, double dt) const throw ();

  /// Write metadata to the file
  void write_meta ( Io * io ) throw ()
  { write_meta_ (meta_type_file, io); }

  /// Write metadata to the current group in the file
  void write_meta_group ( Io * io ) throw ()
  { write_meta_ (meta_type_group, io); }

#ifdef CONFIG_USE_CHARM

  /// Accessor function for the CHARM Loop class
  Loop * loop() { return & loop_; };

  /// Set the index of this output in its simulation
  void set_index_charm(int index_charm) { index_charm_ = index_charm; }

#endif

public: // virtual functions

  /// Initialize next output
  virtual void init () throw()
  {} ;

  /// Open (or create) a file for IO
  virtual void open () throw() = 0;

  /// Close file for IO
  virtual void close () throw() = 0;

  /// Finalize output
  virtual void finalize () throw ()
  { count_ ++; }

  /// Write Simulation data to disk
  virtual void write ( const Simulation * simulation ) throw()
  { write_(simulation); }

  /// Write Hierarchy data to disk
  virtual void write
  ( const Hierarchy * hierarchy, 
    const FieldDescr * field_descr  ) throw()
  { write_(hierarchy,field_descr); }

  /// Write local patch data to disk
  virtual void write
  ( const Patch * patch, 
    const FieldDescr * field_descr,  
    int ixp0=0, int iyp0=0, int izp0=0) throw()
  { write_(patch,field_descr,ixp0,iyp0,izp0); }

  /// Write local block data to disk
  virtual void write
  ( const Block * block, 
    const FieldDescr * field_descr,  
    int ixp0=0, int iyp0=0, int izp0=0) throw()
  { write_(block,field_descr,ixp0,iyp0,izp0); }

  /// Write local field to disk
  virtual void write
  ( const FieldBlock * field_block, 
    const FieldDescr * field_descr,
    int field_index) throw() = 0;

  /// Prepare local array with data to be sent to remote chare for processing
  virtual void prepare_remote (int * n, char ** buffer) throw()
  {};

  /// Accumulate and write data sent from a remote processes
  virtual void update_remote  ( int n, char * buffer) throw()
  {};

  /// Free local array if allocated; NOP if not
  virtual void cleanup_remote (int * n, char ** buffer) throw()
  {};

protected:

  /// "Loop" over writing the Hierarchy in the Simulation
  void write_ (const Simulation * simulation ) throw();

  /// Loop over writing Patches in the Hierarchy
  void write_
  ( const Hierarchy * hierarchy, 
    const FieldDescr * field_descr  ) throw();

  /// Loop over writing Blocks in the Patch
  void write_
  ( const Patch * patch, 
    const FieldDescr * field_descr,  
    int ixp0=0, int iyp0=0, int izp0=0) throw();

  /// Loop over writing Field data in the Block
  void write_
  ( const Block * block, 
    const FieldDescr * field_descr,  
    int ixp0=0, int iyp0=0, int izp0=0) throw();

  /// Return the filename for the file format and given arguments
  std::string expand_file_name_
  (const std::string * file_name,
   const std::vector<std::string> * file_args) const throw();

private:

  /// Implementation of write_meta() and write_meta_group()
  void write_meta_ ( meta_type type, Io * io ) throw();


protected: // attributes

  /// File object for output
  File * file_;

  /// Scheduler for this output
  Schedule * schedule_;

  /// ID of this process
  int process_;

#ifdef CONFIG_USE_CHARM

  /// Loop for ending output
  Loop loop_;

  /// Index of this Output object in Simulation
  size_t index_charm_;

#endif

  /// Simulation cycle for next IO
  int cycle_;

  /// Count of number of times this Output object performed output
  int count_;

  /// Simulation time for next IO
  double time_;

  /// Name of the file to write, including format arguments
  std::string file_name_;

  /// Format strings for file name, if any ("cycle", "time", etc.)
  std::vector<std::string> file_args_;

  /// Iterator over field id's
  ItField * it_field_;

  /// I/O Block data accessor
  IoBlock * io_block_;

  /// I/O FieldBlock data accessor
  IoFieldBlock * io_field_block_;

private: // attributes

  /// Only processes with id's divisible by process_stride_ writes
  /// (1: all processes write; 2: 0,2,4,... write; np: root process writes)
  /// Private so that setting must be made through set_process_stride(),

  int process_stride_;


};

#endif /* IO_OUTPUT_HPP */
