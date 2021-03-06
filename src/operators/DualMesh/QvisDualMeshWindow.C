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

#include "QvisDualMeshWindow.h"

#include <DualMeshAttributes.h>

#include <QLabel>
#include <QLayout>
#include <QButtonGroup>
#include <QRadioButton>


// ****************************************************************************
// Method: QvisDualMeshWindow::QvisDualMeshWindow
//
// Purpose: 
//   Constructor
//
// Note:       Autogenerated by xml2window.
//
// Programmer: xml2window
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

QvisDualMeshWindow::QvisDualMeshWindow(const int type,
                         DualMeshAttributes *subj,
                         const QString &caption,
                         const QString &shortName,
                         QvisNotepadArea *notepad)
    : QvisOperatorWindow(type,subj, caption, shortName, notepad)
{
    atts = subj;
}


// ****************************************************************************
// Method: QvisDualMeshWindow::~QvisDualMeshWindow
//
// Purpose: 
//   Destructor
//
// Note:       Autogenerated by xml2window.
//
// Programmer: xml2window
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

QvisDualMeshWindow::~QvisDualMeshWindow()
{
}


// ****************************************************************************
// Method: QvisDualMeshWindow::CreateWindowContents
//
// Purpose: 
//   Creates the widgets for the window.
//
// Note:       Autogenerated by xml2window.
//
// Programmer: xml2window
// Creation:   omitted
//
// Modifications:
//   Cyrus Harrison, on Aug 18 20:13:20 PDT 2008
//   Qt4 Port - Autogen + Changed labels of radio buttons.   
//
// ****************************************************************************

void
QvisDualMeshWindow::CreateWindowContents()
{
    QGridLayout *mainLayout = new QGridLayout(0);
    topLayout->addLayout(mainLayout);

    modeLabel = new QLabel(tr("Conversion Mode:"), central);
    mainLayout->addWidget(modeLabel,0,0);
    QWidget *modeWidget = new QWidget(central);
    mode = new QButtonGroup(modeWidget);
    QHBoxLayout *modeLayout = new QHBoxLayout(modeWidget);
    modeLayout->setMargin(0);
    modeLayout->setSpacing(10);
    QRadioButton *modeConversionModeAuto = new QRadioButton(tr("Auto"), modeWidget);
    mode->addButton(modeConversionModeAuto,0);
    modeLayout->addWidget(modeConversionModeAuto);
    QRadioButton *modeConversionModeNodesToZones = new QRadioButton(tr("Nodes to Zones"), modeWidget);
    mode->addButton(modeConversionModeNodesToZones,1);
    modeLayout->addWidget(modeConversionModeNodesToZones);
    QRadioButton *modeConversionModeZonesToNodes = new QRadioButton(tr("Zones to Nodes"), modeWidget);
    mode->addButton(modeConversionModeZonesToNodes,2);
    modeLayout->addWidget(modeConversionModeZonesToNodes);
    connect(mode, SIGNAL(buttonClicked(int)),
            this, SLOT(modeChanged(int)));
    mainLayout->addWidget(modeWidget, 0,1);

}


// ****************************************************************************
// Method: QvisDualMeshWindow::UpdateWindow
//
// Purpose: 
//   Updates the widgets in the window when the subject changes.
//
// Note:       Autogenerated by xml2window.
//
// Programmer: xml2window
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

void
QvisDualMeshWindow::UpdateWindow(bool doAll)
{

    for(int i = 0; i < atts->NumAttributes(); ++i)
    {
        if(!doAll)
        {
            if(!atts->IsSelected(i))
            {
                continue;
            }
        }

        switch(i)
        {
          case DualMeshAttributes::ID_mode:
            mode->blockSignals(true);
            if(mode->button((int)atts->GetMode()) != 0)
                mode->button((int)atts->GetMode())->setChecked(true);
            mode->blockSignals(false);
            break;
        }
    }
}


// ****************************************************************************
// Method: QvisDualMeshWindow::GetCurrentValues
//
// Purpose: 
//   Gets values from certain widgets and stores them in the subject.
//
// Note:       Autogenerated by xml2window.
//
// Programmer: xml2window
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

void
QvisDualMeshWindow::GetCurrentValues(int which_widget)
{
}


//
// Qt Slot functions
//


void
QvisDualMeshWindow::modeChanged(int val)
{
    if(val != atts->GetMode())
    {
        atts->SetMode(DualMeshAttributes::ConversionMode(val));
        SetUpdate(false);
        Apply();
    }
}


