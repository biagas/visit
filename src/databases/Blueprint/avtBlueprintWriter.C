/*****************************************************************************
*
* Copyright (c) 2000 - 2019, Lawrence Livermore National Security, LLC
* Produced at the Lawrence Livermore National Laboratory
* LLNL-CODE-442911
* All rights reserved.
*
* This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
* full copyright notice is contained in the file COPYRIGHT located at the root
* of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
*
* Redistribution  and  use  in  source  and  binary  forms,  with  or  without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of  source code must  retain the above  copyright notice,
*    this list of conditions and the disclaimer below.
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
*    documentation and/or other materials provided with the distribution.
*  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
*    be used to endorse or promote products derived from this software without
*    specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
* ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
* LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
* DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
* SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
* CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
* LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
* OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
*
*****************************************************************************/

// ************************************************************************* //
//                               avtBlueprintWriter.C                        //
// ************************************************************************* //

#include <avtBlueprintWriter.h>

#include <vector>

#include <vtkCell.h>
#include <vtkDataSet.h>
#include <vtkDataSetWriter.h>
#include <vtkFieldData.h>
#include <vtkStringArray.h>
#include <vtkIntArray.h>
#include <vtkDoubleArray.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLRectilinearGridWriter.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>

#include <avtDatabaseMetaData.h>
#include <avtParallelContext.h>
#include <FileFunctions.h>

#include <DBOptionsAttributes.h>
#include <DebugStream.h>
#include <ImproperUseException.h>

#ifdef WIN32
#include <direct.h>
#endif

//-----------------------------------------------------------------------------
// conduit includes
//-----------------------------------------------------------------------------
#include "conduit.hpp"
#include "conduit_relay.hpp"
#include "conduit_relay_hdf5.hpp"
#include "conduit_blueprint.hpp"

#include "avtBlueprintDataAdaptor.h"
#include "avtBlueprintLogging.h"

using     std::string;
using     std::vector;

using namespace conduit;

int    avtBlueprintWriter::INVALID_CYCLE = -INT_MAX;
double avtBlueprintWriter::INVALID_TIME = -DBL_MAX;

//-----------------------------------------------------------------------------
// These methods are used to re-wire conduit's default error handling
//-----------------------------------------------------------------------------
void
blueprint_writer_plugin_print_msg(const std::string &msg,
                           const std::string &file,
                           int line)
{
    // Uncomment for very verbose traces:
    //
    // debug5 << "File:"    << file << std::endl
    //        << "Line:"    << line << std::endl
    //        << "Message:" << msg  << std::endl;
    debug5 << msg  << std::endl;
}

//-----------------------------------------------------------------------------
void
blueprint_writer_plugin_info_handler(const std::string &msg,
                              const std::string &file,
                              int line)
{
    blueprint_writer_plugin_print_msg(msg,file,line);
}


//-----------------------------------------------------------------------------
void
blueprint_writer_plugin_warning_handler(const std::string &msg,
                                 const std::string &file,
                                 int line)
{
    blueprint_writer_plugin_print_msg(msg,file,line);
}

//-----------------------------------------------------------------------------
void
blueprint_writer_plugin_error_handler(const std::string &msg,
                               const std::string &file,
                               int line)
{
    std::ostringstream bp_err_oss;
    bp_err_oss << "[ERROR]"
               << "File:"    << file << std::endl
               << "Line:"    << line << std::endl
               << "Message:" << msg  << std::endl;

    debug1 << bp_err_oss.str();

    BP_PLUGIN_EXCEPTION1(InvalidVariableException, bp_err_oss.str());

}

// ****************************************************************************
//  Method: avtBlueprintWriter constructor
//
//  Programmer: Matt Larsen
//  Creation:   Feb 1, 2019
//
//  Modifications:
//
// ****************************************************************************

avtBlueprintWriter::avtBlueprintWriter() :m_stem(), m_meshName()
{
    m_nblocks = 0;

    conduit::utils::set_info_handler(blueprint_writer_plugin_info_handler);
    conduit::utils::set_warning_handler(blueprint_writer_plugin_warning_handler);
    // this catches any uncaught conduit errors, logs them to debug 1
    // and  converts them into a VisIt Exception
    conduit::utils::set_error_handler(blueprint_writer_plugin_error_handler);
}


// ****************************************************************************
//  Method: avtBlueprintWriter::OpenFile
//
//  Purpose:
//      Does no actual work.  Just records the stem name for the files.
//
//  Programmer: Matt Larsen
//  Creation:   Feb 1, 2019
//
//  Modifications:
//
// ****************************************************************************

void
avtBlueprintWriter::OpenFile(const string &stemname, int nb)
{
    m_stem = stemname;
    m_nblocks = nb;

    int c = GetCycle();
    if (c != INVALID_CYCLE)
    {
        c = 0;
    }
    char fmt_buff[64];
    snprintf(fmt_buff, sizeof(fmt_buff), "%06d",c);
    // we want the basename without the extension to use as a sub-dir name
    m_mbDirName = FileFunctions::Basename(m_stem);
    std::ostringstream oss;
    oss << m_stem << ".cycle_" << fmt_buff;
    m_output_dir  =  oss.str();

#ifdef WIN32
    _mkdir(m_output_dir.c_str());
#else
    mkdir(m_output_dir.c_str(), 0777);
#endif
    m_genRoot = true;
    n_root_file.reset();
    BP_PLUGIN_INFO("BlueprintMeshWriter: create output dir "<<m_output_dir);
}


// ****************************************************************************
//  Method: avtBlueprintWriter::WriteHeader
//
//  Purpose:
//      Get any info from the metadata that we need to wrtie data
//
//  Programmer: Matt Larsen
//  Creation:   Feb 1, 2019
//
//  Modifications:
//
// ****************************************************************************

void
avtBlueprintWriter::WriteHeaders(const avtDatabaseMetaData *md,
                           const vector<string> &scalars,
                           const vector<string> &vectors,
                           const vector<string> &materials)
{
    m_meshName = GetMeshName(md);
    m_time     = GetTime();
    m_cycle    = GetCycle();
}


// ****************************************************************************
//  Method: avtBlueprintWriter::WriteChunk
//
//  Purpose:
//      This writes out one chunk of an avtDataset.
//
//  Programmer: Matt Larsen
//  Creation:  Feb 1, 2019
//
//  Modifications:
//
//
// ****************************************************************************
void
avtBlueprintWriter::WriteChunk(vtkDataSet *ds, int chunk)
{
    char chunkname[1024];
    if (m_nblocks > 1)
        sprintf(chunkname, "%s/%s.%d", m_stem.c_str(), m_mbDirName.c_str(), chunk);
    else
        sprintf(chunkname, "%s", m_stem.c_str());

    BP_PLUGIN_INFO("BlueprintMeshWriter: " << chunkname
                    << " [domain " << chunk<< "]");
    Node mesh;
    mesh["state/domain_id"] = chunk;
    //std::string topo_name = "topo";

    if (!m_meshName.empty())
    {
       // topo_name = m_meshName;
    }

    if (m_cycle != INVALID_CYCLE)
    {
        mesh["state/cycle"] = m_cycle;
    }

    if (m_time != INVALID_TIME )
    {
        mesh["state/time"] = m_time;
    }

    int ndims = GetInput()->GetInfo().GetAttributes().GetSpatialDimension();
    avtBlueprintDataAdaptor::BP::VTKToBlueprint(mesh, ds, ndims);

    Node verify_info;
    if(!blueprint::mesh::verify(mesh,verify_info))
    {
         BP_PLUGIN_EXCEPTION1(InvalidVariableException,
                              "VTK to Blueprint convertion failed " << verify_info.to_json());
        return;
    }

    char fmt_buff[64];
    snprintf(fmt_buff, sizeof(fmt_buff), "%06llu",chunk);
    std::stringstream oss;
    oss << "domain_" << fmt_buff << "." << "hdf5";
    string output_file  = conduit::utils::join_file_path(m_output_dir,oss.str());
    relay::io::save(mesh, output_file);

    if(m_genRoot)
    {
        BP_PLUGIN_INFO("BlueprintMeshWriter: generating root");
        GenRootNode(mesh, m_output_dir);
        m_genRoot = false;
    }
}


// ****************************************************************************
//  Method: avtBlueprintWriter::GenRootNode
//
//  Purpose:
//      Generates the conduit node that contains the root file info
//
//  Programmer: Matt Larsen
//  Creation:   Feb 1, 2019
//
//  Modifications:
//
// ****************************************************************************
void
avtBlueprintWriter::GenRootNode(conduit::Node &mesh,
                                const std::string output_dir)
{

    int c = GetCycle();
    if (c != INVALID_CYCLE)
    {
        c = 0;
    }

    char fmt_buff[64];
    snprintf(fmt_buff, sizeof(fmt_buff), "%06llu",c);

    std::stringstream oss;
    std::string root_dir = FileFunctions::Dirname(output_dir);
    oss << m_mbDirName << ".cycle_" << fmt_buff << ".root";
    m_root_file = oss.str();

    m_root_file = utils::join_file_path(root_dir,
                                        m_root_file);

    std::string output_file_pattern;
    output_file_pattern = utils::join_file_path(output_dir,
                                                "domain_%06d.hdf5");
    n_root_file.reset();
    Node &bp_idx = n_root_file["blueprint_index"];
    blueprint::mesh::generate_index(mesh,
                                    "",
                                    m_nblocks,
                                    bp_idx["mesh"]);
    // work around conduit bug
    if(mesh.has_path("state/cycle"))
    {
      bp_idx["mesh/state/cycle"] = mesh["state/cycle"].to_int32();
    }

    if(mesh.has_path("state/time"))
    {
      bp_idx["mesh/state/time"] = mesh["state/time"].to_double();
    }

    n_root_file["protocol/name"]    =  "hdf5";
    n_root_file["protocol/version"] = "0.4.0";

    n_root_file["number_of_files"]  = m_nblocks;
    // for now we will save one file per domain, so trees == files
    n_root_file["number_of_trees"]  = m_nblocks;
    n_root_file["file_pattern"]     = output_file_pattern;
    n_root_file["tree_pattern"]     = "/";

    //n_root_file.print();
}

// ****************************************************************************
//  Method: avtBlueprintWriter::CloseFile
//
//  Purpose:
//      Closes the file.
//
//  Programmer: Matt Larsen
//  Creation:   Feb 1, 2019
//
//  Modifications:
//
// ****************************************************************************

void
avtBlueprintWriter::CloseFile(void)
{
}

// ****************************************************************************
//  Method: avtBlueprintWriter::WriteRootFile
//
//  Purpose:
//      Writes a root file.
//
//  Programmer: Matt Larsen
//  Creation:   Feb 1, 2019
//
//  Modifications:
//
// ****************************************************************************

void
avtBlueprintWriter::WriteRootFile()
{
    int root_writer = 0;
    int rank = 0;
#ifdef PARALLEL
    // assume nothing about what rank was given a chunk;
    rank = writeContext.Rank();
    bool has_root_file = n_root_file.has_path("blueprint_index");
    int i_has_root = has_root_file ? 1 : 0;
    int *roots = new int[writeContext.Size()];

    MPI_Allgather(&i_has_root, 1, MPI_INT, roots, 1, MPI_INT,
                  writeContext.GetCommunicator());

    for(int i = 0; i < writeContext.Size(); ++i)
    {
        if(roots[i] == 1)
        {
            root_writer = i;
            break;
        }
    }

    delete[] roots;
#endif
    if(rank == root_writer)
    {
        BP_PLUGIN_INFO("BlueprintMeshWriter: writing root "<<m_root_file);
        relay::io::save(n_root_file, m_root_file,"hdf5");
    }
}
