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
#include <silo.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// supress the following since silo uses char * in its API
#if defined(__clang__)
# pragma GCC diagnostic ignored "-Wdeprecated-writable-strings"
#elif defined(__GNUC__)
# pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

void
write_fullframe(DBfile *dbfile)
{
    // Write a rectilinear mesh.
    const double xMin = 100.1234e9;
    const double xMax = 152.1234e9;
    const double yMin = 0.;
    const double yMax = M_PI;

    // Number of nodes in x, y
    const int NX = 50;
    const int NY = 50;
    double x[NX], y[NY];

    // Calculate the points.
    int i, j;
    for(i = 0; i < NX; ++i)
    {
        double t = (double)i / (double)(NX-1);
        x[i] = (1.-t) * xMin + t * xMax;
    }
    for(i = 0; i < NY; ++i)
    {
        double t = (double)i / (double)(NY-1);
        y[i] = (1.-t) * yMin + t * yMax;
    }
    int dims[] = {NX, NY};
    int ndims = 2;
    float *coords[2];
    coords[0] = (float *)x;
    coords[1] = (float *)y;
    DBPutQuadmesh(dbfile, "fullframe", NULL, coords, dims, ndims,
                  DB_DOUBLE, DB_COLLINEAR, NULL);

    // Calculate the values for the vector variable.
    double vec0[NY][NX], vec1[NY][NX];
    float *comp[2];
    comp[0] = (float *)vec0;
    comp[1] = (float *)vec1;
    double D = (xMax - xMin) / 100.;
    for(j = 0; j < NY; ++j)
    {
        double anglex = y[j] * 2.;
        for(i = 0; i < NX; ++i)
        {
            vec0[j][i] = D + 2. * D * cos(anglex);
            vec1[j][i] = 0.;
        }
    }

    char *varnames[] = {"vec_comp0", "vec_comp1"};
    dims[0] = NX-1;
    dims[1] = NY-1;
    DBPutQuadvar(dbfile, "vec", "fullframe",
                 2, varnames, comp, dims,
                 ndims, NULL, 0, DB_DOUBLE, DB_ZONECENT, NULL);

    // Create a point mesh
    const int NPTS = NX * NY;
    double xc[NY][NX];
    double yc[NY][NX];
    double rad[NY][NX];
    for(j = 0; j < NY; ++j)
    {
        double ty = (double)j / (double)(NY-1);
        for(i = 0; i < NX; ++i)
        {
            double tx = (double)i / (double)(NX-1);
            xc[j][i] = x[i];
            yc[j][i] = y[j];
            rad[j][i] = sqrt(tx*tx + ty*ty);
        }
    }
    dims[0] = NPTS;
    dims[1] = NPTS;
    coords[0] = (float *)xc;
    coords[1] = (float *)yc;
    DBPutPointmesh(dbfile, "pointmesh", ndims, coords, NPTS,
                   DB_DOUBLE, NULL);
    DBPutPointvar1(dbfile, "rad", "pointmesh", (float*)rad, NPTS,
                   DB_DOUBLE, NULL);

    //
    // Write out a variety of different ways of 1D scalars
    // which VisIt should correctly re-interpret as curves.
    //
    double dc[NX];
    float  fc[NX];
    void *vals[1];
    for (i = 0; i < NX; i++)
    {
        dc[i] = sin(i*2*3.1415/NX);
        fc[i] = (i-NX/2)*5;
    }

    dims[0] = NX;
    ndims = 1;
    coords[0] = (float *)x;
    DBPutQuadmesh(dbfile, "quad_1d", NULL, coords, dims, ndims,
                  DB_DOUBLE, DB_COLLINEAR, NULL);
    varnames[0] = "qd_1d_comp0";
    dims[0] = NX;
    vals[0] = dc;
    DBPutQuadvar(dbfile, "qd_1d", "quad_1d",
                 1, varnames, (float**) vals, dims,
                 ndims, NULL, 0, DB_DOUBLE, DB_NODECENT, NULL);
    varnames[0] = "qu_1d_comp1";
    dims[0] = NX;
    vals[0] = fc;
    DBPutQuadvar(dbfile, "qu_1d", "quad_1d",
                 1, varnames, (float**) vals, dims,
                 ndims, NULL, 0, DB_FLOAT, DB_NODECENT, NULL);

    dims[0] = NX;
    coords[0] = (float *)xc;
    vals[0] = dc;
    DBPutPointmesh(dbfile, "point_1d", ndims, coords, NX,
                   DB_DOUBLE, NULL);
    DBPutPointvar1(dbfile, "rad_1d", "point_1d", (float*)vals[0], NX,
                   DB_DOUBLE, NULL);

    int *zonelist = new int[(NX-1)*2];
    int shapesize = 2;
    int shapecnt = NX-1;
    int shapetype = DB_ZONETYPE_BEAM;
    for (i = 0; i < NX-1; i++)
    {
        zonelist[2*i] = i;
        zonelist[2*i+1] = i+1;
    }
    DBPutZonelist2(dbfile, "ucd_1d_zl", NX-1, 1, zonelist, (NX-1)*2,
                0, 0, 0, &shapetype, &shapesize, &shapecnt, 1, NULL);
    delete [] zonelist;
 
    char *coordnames = "ucd_x";
    coords[0] = (float *)x;
    DBPutUcdmesh(dbfile, "ucd_1d", 1, &coordnames, coords,
                NX, NX-1, "ucd_1d_zl", NULL, DB_DOUBLE, NULL);

    varnames[0] = "u";
    vals[0] = dc;
    DBPutUcdvar(dbfile, "ud_1d", "ucd_1d", 1, varnames, (float**)vals,
        NX, NULL, 0, DB_DOUBLE, DB_NODECENT, 0);
    vals[0] = fc;
    DBPutUcdvar(dbfile, "uf_1d", "ucd_1d", 1, varnames, (float**)vals,
        NX, NULL, 0, DB_FLOAT, DB_NODECENT, 0);
}

int
main(int argc, char *argv[])
{
    // Check for the right driver.
    int driver = DB_PDB;
    for(int j = 1; j < argc; ++j)
    {
        if (strcmp(argv[j], "DB_HDF5") == 0)
            driver = DB_HDF5;
        else if (strcmp(argv[j], "DB_PDB") == 0)
            driver = DB_PDB;
        else
           fprintf(stderr,"Uncrecognized driver name \"%s\"\n", argv[j]);
    }

    // Open the Silo file
    DBfile *dbfile = DBCreate("fullframe.silo", DB_CLOBBER, DB_LOCAL,
        "2D meshes with extents that will cause "
        "VisIt to automatically enter fullframe mode", driver);
    if(dbfile == NULL)
    {
        fprintf(stderr, "Could not create Silo file!\n");
        return -1;
    }

    // Add other Silo calls here.
    write_fullframe(dbfile);

    // Close the Silo file.
    DBClose(dbfile);
    return 0;
}
