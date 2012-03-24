// See LICENSE_CELLO file for license and copyright information

/// @file     io_Input.cpp
/// @author   James Bordner (jobordner@ucsd.edu)
/// @date     Wed Mar 16 09:53:31 PDT 2011
/// @brief    Implementation of the Input class

#include "cello.hpp"

#include "io.hpp"

//----------------------------------------------------------------------

Input::Input (const Factory * factory) throw()
  : file_(0),           // Initialization deferred
    process_stride_(1), // default one file per process
    process_(0),        // initialization below
#ifdef CONFIG_USE_CHARM
    count_reduce_(0),
#endif
    cycle_(0),
    count_input_(0),
    time_(0),
    file_name_(""),     // set_filename()
    file_args_(),       // set_filename()
    it_field_(0),        // set_it_field()
    io_block_(0),
    io_field_block_(0)
{

  GroupProcess * group_process = GroupProcess::create();
  process_  = group_process->rank();
  delete group_process;

  io_block_       = factory->create_io_block();
  io_field_block_ = factory->create_io_field_block();
}

//----------------------------------------------------------------------

Input::~Input () throw()
{
  delete file_;      file_ = 0;
  delete it_field_;  it_field_ = 0;
  delete io_block_;  io_block_ = 0;
}
//----------------------------------------------------------------------

void Input::set_filename (std::string filename,
			   std::vector<std::string> fileargs) throw()
{
  file_name_ = filename; 
  file_args_ = fileargs;
}

//----------------------------------------------------------------------


std::string Input::expand_file_name 
(
 const std::string              * file_name_p,
 const std::vector<std::string> * file_args_p
) const throw()
{
  const std::string & file_name = *file_name_p;
  const std::vector<std::string> & file_args = *file_args_p;
  
  const int MAX_BUFFER = 255;

  char buffer[MAX_BUFFER];
  char buffer_new[MAX_BUFFER];

  // Error check no \% in file name

  ASSERT1 ("Input::expand_file_name",
	   "File name %s cannot contain '\\%%'",
	   file_name.c_str(),
	   file_name.find("\\%") == std::string::npos);

  // Error check variable count equals format conversion specifier count

  std::string file_rest = file_name;
  size_t count = 0;
  size_t pos = 0;
  size_t len;
  while ((pos = file_rest.find("%")) != std::string::npos) {
    count ++;
    len = file_rest.size();
    file_rest = file_rest.substr(pos+1,len-pos-1);
  }

  ASSERT3 ("Input::expand_file_name",
	   "The number of format conversion specifiers %d "
	   "associated with file name %s "
	   "must equal the number of variables %d",
	    count, file_name.c_str(),file_args.size(),
	   file_args.size() == count);

  // loop through file_args[] from the right and replace 
  // format strings with variable values

  std::string file_left  = file_name;
  std::string file_middle = "";
  std::string file_right = "";

  for (size_t i=0; i<file_args.size(); i++) {

    // visit variables from right to left
    const std::string & arg = file_args[file_args.size() - i - 1];

    size_t pos = file_left.rfind("%");
    size_t len = file_left.size();

    file_middle = file_left.substr(pos,len-pos);
    file_left  = file_left.substr(0,pos);

    strcpy (buffer, file_middle.c_str());
    
    if      (arg == "cycle") { sprintf (buffer_new,buffer, cycle_); }
    else if (arg == "time")  { sprintf (buffer_new,buffer, time_); }
    else if (arg == "count") { sprintf (buffer_new,buffer, count_input_); }
    else if (arg == "proc")  { sprintf (buffer_new,buffer, process_); }
    else 
      {
	ERROR3("Input::expand_file_name",
	       "Unknown file variable #%d '%s' for file '%s'",
	       int(i),arg.c_str(),file_name.c_str());
      }

    file_right = std::string(buffer_new) + file_right;

  }

  return file_left + file_right;
}

//----------------------------------------------------------------------

void Input::read_simulation
(
 Simulation * simulation
 ) throw()
{
  WARNING("Input::read_simulation()",
	  "This function should not be called");
}

//----------------------------------------------------------------------

void Input::read_hierarchy
(
 Hierarchy * hierarchy,
 const FieldDescr * field_descr
) throw()
{

  ItPatch it_patch (hierarchy);

  // (*) read data patch_list_

  while (Patch * patch = ++it_patch) {

    // NO OFFSET: ASSUMES ROOT PATCH
    read_patch (patch, field_descr,  0,0,0);

  }
}

//----------------------------------------------------------------------

void Input::read_patch 
(
 Patch * patch,
 const FieldDescr * field_descr,
 int ixp0, int iyp0, int izp0
 ) throw()

{

#ifdef CONFIG_USE_CHARM

  // CHARM++ Block callback for read_block()

  if (patch->blocks_allocated()) {
    patch->block_array().p_read (index_charm_);
  }

#else

  ItBlock it_block (patch);
  while (Block * block = ++it_block) {

    // NO OFFSET: ASSUMES ROOT PATCH
    read_block (block, field_descr, 0,0,0);

  }
#endif
}

//----------------------------------------------------------------------

void Input::read_block
(
 Block * block,
 const FieldDescr * field_descr,
 int ixp0, int iyp0, int izp0
 ) throw()
{
  // Read fields

  for (it_field_->first(); ! it_field_->done(); it_field_->next()  ) {
    int field_index = it_field_->value();
    read_field (block->field_block(), field_descr, field_index);
  }
}

