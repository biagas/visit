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
//                               QtVisWindow.C                               //
// ************************************************************************* //

#include <QtVisWindow.h>

#include <VisWinRenderingWithWindow.h>
#include <vtkQtRenderWindow.h>
#include <VisWinTools.h>

vtkQtRenderWindow* (*QtVisWindow::windowCreationCallback)(void *) = 0;
void                *QtVisWindow::windowCreationData = 0;
bool                QtVisWindow::ownsAllWindows = false;

// ****************************************************************************
//  Method: QtVisWindow constructor
//
//  Programmer: Hank Childs
//  Creation:   March 4, 2004
//
//  Modifications:
//    Jeremy Meredith, Tue Jul 17 16:35:37 EDT 2007
//    Added fullscreen support.
//
//    Brad Whitlock, Fri Apr  6 11:43:24 PDT 2012
//    Pass stereo flag to vtkQtRenderWindow constructor so we can create it
//    with the right stereo flags set on the GL context.
//
// ****************************************************************************

QtVisWindow::QtVisWindow(bool fullScreenMode) : VisWindow(false)
{
    bool owns = true;
    vtkQtRenderWindow *renWin = 0;
    if(windowCreationCallback != 0)
    {
        renWin = windowCreationCallback(windowCreationData);
        owns = ownsAllWindows; //false;
    }
    else
        renWin = vtkQtRenderWindow::New(VisWinRendering::GetStereoEnabled());
    VisWinRenderingWithWindow *ren =
        new VisWinRenderingWithWindow(renWin, owns, colleagueProxy);
    ren->SetFullScreenMode(fullScreenMode);
    Initialize(ren);
}

// ****************************************************************************
// Method: QtVisWindow::SetWindowCreationCallback
//
// Purpose: 
//   Sets the window creation callback.
//
// Arguments:
//   wcc     : The window creation callback.
//   wccdata : The window creation callback arguments.
//
// Returns:    A new instance of vtkQtRenderWindow.
//
// Note:       
//
// Programmer: Brad Whitlock
// Creation:   Mon Aug 18 16:24:18 PDT 2008
//
// Modifications:
//   
// ****************************************************************************

void
QtVisWindow::SetWindowCreationCallback(vtkQtRenderWindow *(*wcc)(void*), void *wccdata)
{
    windowCreationCallback = wcc;
    windowCreationData = wccdata;
}

// ****************************************************************************
// Method: QtVisWindow::SetOwnerShipOfWindows
//
// Purpose:
//   Sets the owner of the windows.
//
// Arguments:
//   isowner     : True: VisIt owns the windows, False: User owns the windows.
//
// Returns:    None.
//
// Note:
//
// Programmer: Hari Krishnan
// Creation:   Mon Oct 25 16:24:18 PDT 2011
//
// Modifications:
//
// ****************************************************************************

void
QtVisWindow::SetOwnerShipOfAllWindows(bool isowner)
{
    ownsAllWindows = isowner;
}


// ****************************************************************************
// Method: QtVisWindow::CreateToolColleague
//
// Purpose: 
//   Create tools colleague with tool geometry.
//
// Arguments:
//
// Returns:    
//
// Note:       
//
// Programmer: Brad Whitlock
// Creation:   Tue Dec 7 16:56:07 PST 2010
//
// Modifications:
//   
// ****************************************************************************

void
QtVisWindow::CreateToolColleague()
{
    tools = new VisWinTools(colleagueProxy, true);
    AddColleague(tools);
}
