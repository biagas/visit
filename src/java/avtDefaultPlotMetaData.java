// ***************************************************************************
//
// Copyright (c) 2000 - 2010, Lawrence Livermore National Security, LLC
// Produced at the Lawrence Livermore National Laboratory
// LLNL-CODE-400124
// All rights reserved.
//
// This file is  part of VisIt. For  details, see https://visit.llnl.gov/.  The
// full copyright notice is contained in the file COPYRIGHT located at the root
// of the VisIt distribution or at http://www.llnl.gov/visit/copyright.html.
//
// Redistribution  and  use  in  source  and  binary  forms,  with  or  without
// modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of  source code must  retain the above  copyright notice,
//    this list of conditions and the disclaimer below.
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this  list of  conditions  and  the  disclaimer (as noted below)  in  the
//    documentation and/or other materials provided with the distribution.
//  - Neither the name of  the LLNS/LLNL nor the names of  its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR  IMPLIED WARRANTIES, INCLUDING,  BUT NOT  LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS FOR A PARTICULAR  PURPOSE
// ARE  DISCLAIMED. IN  NO EVENT  SHALL LAWRENCE  LIVERMORE NATIONAL  SECURITY,
// LLC, THE  U.S.  DEPARTMENT OF  ENERGY  OR  CONTRIBUTORS BE  LIABLE  FOR  ANY
// DIRECT,  INDIRECT,   INCIDENTAL,   SPECIAL,   EXEMPLARY,  OR   CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR
// SERVICES; LOSS OF  USE, DATA, OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER
// CAUSED  AND  ON  ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT
// LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY  WAY
// OUT OF THE  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// ***************************************************************************

package llnl.visit;

import java.util.Vector;

// ****************************************************************************
// Class: avtDefaultPlotMetaData
//
// Purpose:
//    Contains default plot metadata attributes
//
// Notes:      Autogenerated by xml2java.
//
// Programmer: xml2java
// Creation:   omitted
//
// Modifications:
//   
// ****************************************************************************

public class avtDefaultPlotMetaData extends AttributeSubject
{
    private static int numAdditionalAttributes = 3;

    public avtDefaultPlotMetaData()
    {
        super(numAdditionalAttributes);

        pluginID = new String("");
        plotVar = new String("var");
        plotAttributes = new Vector();
    }

    public avtDefaultPlotMetaData(int nMoreFields)
    {
        super(numAdditionalAttributes + nMoreFields);

        pluginID = new String("");
        plotVar = new String("var");
        plotAttributes = new Vector();
    }

    public avtDefaultPlotMetaData(avtDefaultPlotMetaData obj)
    {
        super(numAdditionalAttributes);

        int i;

        pluginID = new String(obj.pluginID);
        plotVar = new String(obj.plotVar);
        plotAttributes = new Vector(obj.plotAttributes.size());
        for(i = 0; i < obj.plotAttributes.size(); ++i)
            plotAttributes.addElement(new String((String)obj.plotAttributes.elementAt(i)));


        SelectAll();
    }

    public int Offset()
    {
        return super.Offset() + super.GetNumAdditionalAttributes();
    }

    public int GetNumAdditionalAttributes()
    {
        return numAdditionalAttributes;
    }

    public boolean equals(avtDefaultPlotMetaData obj)
    {
        int i;

        // Compare the elements in the plotAttributes vector.
        boolean plotAttributes_equal = (obj.plotAttributes.size() == plotAttributes.size());
        for(i = 0; (i < plotAttributes.size()) && plotAttributes_equal; ++i)
        {
            // Make references to String from Object.
            String plotAttributes1 = (String)plotAttributes.elementAt(i);
            String plotAttributes2 = (String)obj.plotAttributes.elementAt(i);
            plotAttributes_equal = plotAttributes1.equals(plotAttributes2);
        }
        // Create the return value
        return ((pluginID.equals(obj.pluginID)) &&
                (plotVar.equals(obj.plotVar)) &&
                plotAttributes_equal);
    }

    // Property setting methods
    public void SetPluginID(String pluginID_)
    {
        pluginID = pluginID_;
        Select(0);
    }

    public void SetPlotVar(String plotVar_)
    {
        plotVar = plotVar_;
        Select(1);
    }

    public void SetPlotAttributes(Vector plotAttributes_)
    {
        plotAttributes = plotAttributes_;
        Select(2);
    }

    // Property getting methods
    public String GetPluginID() { return pluginID; }
    public String GetPlotVar() { return plotVar; }
    public Vector GetPlotAttributes() { return plotAttributes; }

    // Write and read methods.
    public void WriteAtts(CommunicationBuffer buf)
    {
        if(WriteSelect(0, buf))
            buf.WriteString(pluginID);
        if(WriteSelect(1, buf))
            buf.WriteString(plotVar);
        if(WriteSelect(2, buf))
            buf.WriteStringVector(plotAttributes);
    }

    public void ReadAtts(int index, CommunicationBuffer buf)
    {
        switch(index)
        {
        case 0:
            SetPluginID(buf.ReadString());
            break;
        case 1:
            SetPlotVar(buf.ReadString());
            break;
        case 2:
            SetPlotAttributes(buf.ReadStringVector());
            break;
        }
    }

    public String toString(String indent)
    {
        String str = new String();
        str = str + stringToString("pluginID", pluginID, indent) + "\n";
        str = str + stringToString("plotVar", plotVar, indent) + "\n";
        str = str + stringVectorToString("plotAttributes", plotAttributes, indent) + "\n";
        return str;
    }


    // Attributes
    private String pluginID;
    private String plotVar;
    private Vector plotAttributes; // vector of String objects
}

