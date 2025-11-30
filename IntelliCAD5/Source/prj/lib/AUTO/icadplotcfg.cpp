/* F:\DEV\PRJ\LIB\AUTO\ICADPLOTCFG.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * <TODO : Add Abstract here>
 * 
 */ 

#include "stdafx.h"
#include "icadplotcfg.h"
#include "sds.h"
#include "win32misc.h"
#include "Preferences.h"
#include "winspool.h"

#include <stdlib.h>
#include <string.h>


// The routines are for reading and writing plot pen/color information to and
// from the Registry, an acad .pcp file and an icad. icp file. They are meant
// to be used by the pen setting dialog box yet to be created.

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
// TODO
// There is one MessageBox() call in exportPCP() that has it's owner as the
// IntelliCAD main window. It may be better to make the plot dialog the owner.

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

static CString getPlotCfgRegistryRoot()
{
    CString SDS_RegEntry;
    VERIFY(getRegCurrentProfileRoot(SDS_RegEntry.GetBuffer(IC_ACADBUF), IC_ACADBUF));
	SDS_RegEntry.ReleaseBuffer();
	SDS_RegEntry += "\\Config\\plot";
	return SDS_RegEntry;
}

//----------------------------------------------------------------------------
PlotCfg::PlotCfg()
{
        version = 1.0;
        penwidth  = 0.1000;
        origin[0] = origin[1] = 0.0;

        rotate = 0;
        optimizelevel = 4;

        _tcscpy(units,      "_I"/*DNT*/);
        _tcscpy(hide,       "_N"/*DNT*/);
        _tcscpy(plotfile,   "_NONE"/*DNT*/);
        _tcscpy(filladjust, "_N"/*DNT*/);
        _tcscpy(size,       "MAX"/*DNT*/);
        _tcscpy(scale,      "_F"/*DNT*/);

        // 0 index should not be used
        pens[0].color    = 0;
        pens[0].linetype = 0;
        pens[0].number   = 0;
        pens[0].speed    = 0;
        pens[0].weight   = 0.0;
        
        for (int i = 1; i < IC_PENSBUF; i++) {
            pens[i].color    = i;
            pens[i].linetype = 0;
            pens[i].number   = i;
            pens[i].speed    = 36;
            pens[i].weight   = 0.01000;
        }

        //registry/print/plot only - not pcp file
        area = CurrentView;
        fromX = 0;
        fromY = 0;
        toX = 0;
        toY = 0;
        _tcscpy(view, ""/*DNT*/);
        _tcscpy(header, ""/*DNT*/);
        _tcscpy(footer, ""/*DNT*/);
        onlyWindowArea = FALSE;
        centerOnPage = FALSE;
		// EBATECH(CNBR) 24-03-2003 Bugzilla#78471 Support Lineweight
		useLWeight = FALSE;
		scaleLWeight = FALSE;

	*m_printerName = 0;
	// Invalid value -1 indicates that we don't override the according value in DEVMODE structure,
	// see CIcadApp::getPrinterSettingsFromPlotcfg.
	m_paperOrientation = m_paperSize = m_paperWidth = m_paperLength = m_paperSource = -1;
	getSystemDefaultPrinterSettings();
}

//----------------------------------------------------------------------------
PlotCfg::~PlotCfg()
{
}

//----------------------------------------------------------------------------
int PlotCfg::importPCP( LPCTSTR FileName)
{
    char tmpBuffer[IC_PENSBUF], *loc;

    if (!fileExists(FileName)) {
        return RTERROR;
    }

    FILE* filePtr;
    // Now open it for reading.
    if ((filePtr = fopen(FileName, "r"/*DNT*/)) == NULL) {
        sprintf(tmpBuffer, "Error opening: %s for reading.\n", FileName);
        sds_alert(tmpBuffer);
        return RTERROR;
    }

    int index;
    // Using a static size here because I'm using strtok which
    // is destructive, and I propably will introduce a leak if 
    // I allocate the size.
    char fileBuffer[PCPFILESIZE], *bufferPtr;
    memset(fileBuffer, 0, sizeof(fileBuffer));
    bufferPtr = fileBuffer;
    fread(fileBuffer,sizeof(fileBuffer), 1, filePtr);
    fclose(filePtr);

    char *line;
    line = _tcstok(bufferPtr, EOL);

    // Read the header stuff.
    while(_tcsstr(line, "BEGIN_COLOR"/*DNT*/) == NULL && line != NULL) {

        if (line != NULL) {
            if ( _tcsstr(line, "VERSION"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    version = atof(loc + 1);
            }
            if ( _tcsstr(line, "UNITS"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    _tcsncpy(units, loc + 1, SMALLBUF-1);
            }
            if ( _tcsstr(line, "ORIGIN"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL) 
                    _tcsncpy(tmpBuffer, loc + 1, SMALLBUF-1);
                loc = _tcsrchr(tmpBuffer, COMMACHR);
                if (loc != NULL)
                    *(loc) = EOSCHR;
                origin[0] = atof(tmpBuffer);
                loc = _tcsrchr(line, COMMACHR);
                if (loc != NULL)
                    origin[1] = atof(loc + 1);            
            }
            if ( _tcsstr(line, "SIZE"/*DNT*/) != NULL) {
                _tcsncpy(size, "MAX"/*DNT*/, SMALLBUF-1);
            }
            if ( _tcsstr(line, "ROTATE"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    rotate = atoi(loc + 1);
            }
            if ( _tcsstr(line, "HIDE"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    _tcsncpy(hide, loc + 1, SMALLBUF-1);
            }
            if ( _tcsstr(line, "PEN_WIDTH"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    penwidth = atof(loc + 1);
            }
            if ( _tcsstr(line, "SCALE"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    _tcsncpy(scale, loc + 1, SMALLBUF-1);
            }
            if ( _tcsstr(line, "PLOT_FILE"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    _tcsncpy(plotfile, loc + 1, SMALLBUF-1);
            }
            if ( _tcsstr(line, "FILL_ADJUST"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    _tcsncpy(filladjust, loc + 1, SMALLBUF-1);
            }
            if ( _tcsstr(line, "OPTIMIZE_LEVEL"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    optimizelevel = atoi(loc + 1);
            }
        }

        line = _tcstok(NULL, EOL);
    }
    
    // Now we have the firt Header variables;
    while(line != NULL) {
        if (_tcsstr(line, "END_COLOR"/*DNT*/) != NULL) {
            line = _tcstok(NULL, EOL);
            continue;
        }

        // color
        if (_tcsstr(line, "BEGIN_COLOR"/*DNT*/) != NULL) {
            loc = _tcsrchr(line, SPACECHR);
            if (loc != NULL)
                index = atoi(loc + 1);
            pens[index].color = index;
        } 
        // pen_number.
        if (_tcsstr(line, "PEN_NUMBER"/*DNT*/) != NULL) {
            if (line != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    pens[index].number = atoi(loc + 1);
            }
        } 
        // hw_linetype.
        if (_tcsstr(line, "HW_LINETYPE"/*DNT*/) != NULL) {
            if (line != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    pens[index].linetype = atoi(loc + 1);
            }
        } 
        // pen_speed.
        pens[index].speed = 0;
        if (_tcsstr(line, "PEN_SPEED"/*DNT*/) != NULL) {
            if (line != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    pens[index].speed = atoi(loc + 1);
            }
        } 
        // pen_weight.
        if (_tcsstr(line, "PEN_WEIGHT"/*DNT*/) != NULL) {
            if (line != NULL) {
                loc = _tcsrchr(line, SPACECHR);
                if (loc != NULL)
                    pens[index].weight = (float)atof(loc + 1);
            }
        } 

        line = _tcstok(NULL, EOL);  // NEXT FIELD.
    }

    return RTNORM;
}

//----------------------------------------------------------------------------
int PlotCfg::exportPCP( LPCTSTR FileName)
{
    char tmpBuffer[IC_PENSBUF];

    if (FileName == NULL) {
        return RTERROR;
    }

    if (fileExists(FileName)) {
        sprintf(tmpBuffer, "File %s already exists. Overwrite ? <Y>", FileName);
        
        // Just left the error message if stand alone.  This check will be handled in the 
        // print dialog
#ifdef STANDALONE
        if (MessageBox(NULL, tmpBuffer, "File Exists Error", MB_YESNO) == IDNO) 
            return RTERROR;
#endif
    }
    
    FILE* filePtr;
    // Now open it for writing destroying any existing data.
    if ((filePtr = fopen(FileName, "w+"/*DNT*/)) == NULL) {
        sprintf(tmpBuffer, "Error opening: %s for reading.\n", FileName);
        sds_alert(tmpBuffer);
        return RTERROR;
    }

    // First we'll write the header.
    sprintf(tmpBuffer,"VERSION = %.1f\n"/*DNT*/, version);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "UNITS = %s\n"/*DNT*/,units);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "ORIGIN = %.2f,%.2f\n"/*DNT*/,origin[0], origin[1]);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "SIZE = %s\n"/*DNT*/,size/*DNT*/);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "ROTATE = %d\n"/*DNT*/,rotate);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "HIDE = %s\n"/*DNT*/,hide);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "PEN_WIDTH = %f\n"/*DNT*/,penwidth);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "SCALE = %s\n"/*DNT*/,scale);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "PLOT_FILE = %s\n"/*DNT*/,plotfile);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "FILL_ADJUST = %s\n"/*DNT*/,filladjust);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "OPTIMIZE_LEVEL = %d\n\n"/*DNT*/,optimizelevel);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );
    
    // Now write out the pens.
    for (int i = 1; i < IC_PENSBUF; i++) {
        sprintf(tmpBuffer, "BEGIN_COLOR = %d\n"/*DNT*/, pens[i].color);
        fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

        sprintf(tmpBuffer, "  PEN_NUMBER = %d\n"/*DNT*/, pens[i].number);
        fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

        sprintf(tmpBuffer, "  HW_LINETYPE = %d\n"/*DNT*/, pens[i].linetype);
        fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

        sprintf(tmpBuffer, "  PEN_SPEED = %d\n"/*DNT*/, pens[i].speed);
        fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

        sprintf(tmpBuffer, "  PEN_WEIGHT = %f\n"/*DNT*/, pens[i].weight);
        fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

        sprintf(tmpBuffer, "END_COLOR\n\n"/*DNT*/);
        fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );
    }

    fclose(filePtr);
    return RTNORM;
}

//----------------------------------------------------------------------------
int PlotCfg::importICP( LPCTSTR FileName)
{
    char tmpBuffer[IC_PENSBUF], *loc;

    if (FileName == NULL) {
        return RTERROR;
    }

    FILE* filePtr;
    // Now open it for reading.
    if ((filePtr = fopen(FileName, "r"/*DNT*/)) == NULL) {
        sprintf(tmpBuffer, "Error opening: %s for reading.\n", FileName);
        sds_alert(tmpBuffer);
        return RTERROR;
    }

    // Using a static size here because I'm using strtok which
    // is destructive, and I propably will introduce a leak if 
    // I allocate the size.
    char fileBuffer[PCPFILESIZE], *bufferPtr;
    memset(fileBuffer, 0, sizeof(fileBuffer));
    bufferPtr = fileBuffer;
    fread(fileBuffer,PCPFILESIZE, 1, filePtr);
    fclose(filePtr);

    char *line;
    // Get rid of the header stuff.
    line = _tcstok(bufferPtr, EOL);

    // Read the header stuff.
    while(_tcsstr(line, "[Color-1]"/*DNT*/) == NULL) {
        if (line != NULL) {
            if (_tcsstr(line, "Version"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL)
                    version = atof(loc + 1);
            }
            if (_tcsstr(line, "Units"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL)
                    _tcscpy(units,loc + 1);
            }
            if (_tcsstr(line, "Origin"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL) 
                    _tcsncpy(tmpBuffer, loc + 1, SMALLBUF-1);
                loc = _tcsrchr(tmpBuffer, COMMACHR);
                if (loc != NULL)
                    *(loc) = EOSCHR;
                origin[0] = atof(tmpBuffer);
                loc = _tcsrchr(line, COMMACHR);
                if (loc != NULL)
                    origin[1] = atof(loc + 1);            
            }
            if (_tcsstr(line, "Size"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL)
                    _tcscpy(size,loc + 1);
            }
            if (_tcsstr(line, "Rotate"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL)
                    rotate = atoi(loc + 1);
            }
            if (_tcsstr(line, "Hide"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL)
                    _tcscpy(hide,loc + 1);
            }
            if (_tcsstr(line, "PenWidth"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL)
                    penwidth = atof(loc + 1);
            }
            if (_tcsstr(line, "Scale"/*DNT*/) != NULL) {
                loc = _tcsstr(line, EQUALSTR);
                if (loc != NULL) 
                    _tcsncpy(scale, loc + 1, SMALLBUF-1);
            }
            if (_tcsstr(line, "PlotFile"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL)
                    _tcscpy(plotfile,loc + 1);
            }
            if (_tcsstr(line, "FillAdjust"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL)
                    _tcscpy(filladjust,loc + 1);
            }
            if (_tcsstr(line, "OptimizeLevel"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL)
                    optimizelevel = atoi(loc + 1);
            }
			// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
            if (_tcsstr(line, "UseLWeight"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL)
                    useLWeight = atoi(loc + 1);
            }
            if (_tcsstr(line, "ScaleLWeight"/*DNT*/) != NULL) {
                loc = _tcsrchr(line, EQUALCHR);
                if (loc != NULL)
                    scaleLWeight = atoi(loc + 1);
            }
			// EBATECH(CNBR) ]-
        }
        line = _tcstok(NULL, EOL);
    }   

    // Now we are at the pen settings.
    int index;
    while(line != NULL) {
        // color
        if (_tcsstr(line, "[Color-"/*DNT*/) != NULL) {
            loc = _tcsrchr(line, DASHCHR);
            if (loc != NULL)
                _tcscpy(tmpBuffer, loc + 1);
            loc = _tcsrchr(tmpBuffer, BRACKETCHR);
            if (loc != NULL)
                *(loc) = EOSCHR;
            index = atoi(tmpBuffer);
            pens[index].color = index;
        } 
        if (_tcsstr(line, "PenNumber"/*DNT*/) != NULL) {
            loc = _tcsrchr(line, EQUALCHR);
            pens[index].number = atoi(loc + 1);
        } 
        if (_tcsstr(line, "LineType"/*DNT*/) != NULL) {
            loc = _tcsrchr(line, EQUALCHR);
            pens[index].linetype = atoi(loc + 1);
        } 
        if (_tcsstr(line, "PenSpeed"/*DNT*/) != NULL) {
            loc = _tcsrchr(line, EQUALCHR);
            pens[index].speed = atoi(loc + 1);
        } 
        if (_tcsstr(line, "PenWeight"/*DNT*/) != NULL) {
            loc = _tcsrchr(line, EQUALCHR);
            pens[index].weight = atof(loc + 1);
        } 

        line = _tcstok(NULL, EOL);
    }

    fclose(filePtr);
    return RTNORM;
}

//----------------------------------------------------------------------------
int PlotCfg::exportICP( LPCTSTR FileName)
{
    char tmpBuffer[IC_PENSBUF];

    if (FileName == NULL) {
        return RTERROR;
    }

    FILE* filePtr;
    // Now open it for reading.
    if ((filePtr = fopen(FileName, "w"/*DNT*/)) == NULL) {
            fprintf(stdout, "Error opening: %s for reading.\n", FileName);
            return RTERROR;
    }
    // First we'll write the header.
    sprintf(tmpBuffer, "[IntelliCAD Custom Plot Configuration File]\n"/*DNT*/);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tVersion=%.2f\n"/*DNT*/,version);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tUnits=%s\n"/*DNT*/,units);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tOrigin=%.2f,%.2f\n"/*DNT*/,origin[0], origin[1]);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tSize=%s\n"/*DNT*/, size);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tRotate=%d\n"/*DNT*/, rotate);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tHide=%s\n"/*DNT*/, hide);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tPenWidth=%f\n"/*DNT*/, penwidth);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tScale=%s\n"/*DNT*/,scale);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tPlotFile=%s\n"/*DNT*/,plotfile);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tFillAdjust=%s\n"/*DNT*/,filladjust);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tOptimizeLevel=%d\n"/*DNT*/,optimizelevel);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
    sprintf(tmpBuffer, "\tUseLWeight=%d\n"/*DNT*/,(int)useLWeight);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

    sprintf(tmpBuffer, "\tScaleLWeight=%d\n"/*DNT*/,(int)scaleLWeight);
    fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );
	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight

    for (int i = 1;i < IC_PENSBUF; i++) {
        sprintf(tmpBuffer, "[Color-%d]\n"/*DNT*/, pens[i].color);
        fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

        sprintf(tmpBuffer, "\tPenNumber=%d\n"/*DNT*/, pens[i].number);
        fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

        sprintf(tmpBuffer, "\tLineType=%d\n"/*DNT*/, pens[i].linetype);
        fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

        sprintf(tmpBuffer, "\tPenSpeed=%d\n"/*DNT*/, pens[i].speed);
        fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );

		sprintf(tmpBuffer, "\tPenWeight=%.15f\n"/*DNT*/, pens[i].weight);
        fwrite(tmpBuffer, sizeof(char), strlen(tmpBuffer), filePtr );
    }
    fclose(filePtr);
    return RTNORM;
}

//----------------------------------------------------------------------------
int PlotCfg::writePlotConfigToReg()
{
    HKEY hKey;
    DWORD retval;
    char key[10];
    char tmpBuffer[IC_PENSBUF*2];

    CString SDS_RegEntry = getPlotCfgRegistryRoot();

    if(ERROR_SUCCESS!=RegCreateKeyEx(HKEY_CURRENT_USER,
                   SDS_RegEntry,
                   0,""/*DNT*/,REG_OPTION_NON_VOLATILE,
                    KEY_ALL_ACCESS,NULL,&hKey,&retval)) {
        return(RTERROR);
    }
    sprintf(tmpBuffer, "%f", version);
    RegSetValueEx(hKey,"Version"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%s", units);
    RegSetValueEx(hKey,"Units"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%f,%f", origin[0], origin[1]);
    RegSetValueEx(hKey,"Origin"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%s", size);
    RegSetValueEx(hKey,"Size"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%d", rotate);
    RegSetValueEx(hKey,"Rotate"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%s", hide);
    RegSetValueEx(hKey,"Hide"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%f", penwidth);
    RegSetValueEx(hKey,"PenWidth"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%s", scale);
    RegSetValueEx(hKey,"Scale"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%s", plotfile);
    RegSetValueEx(hKey,"PlotFile"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%s", filladjust);
    RegSetValueEx(hKey,"FillAdjust"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%d", optimizelevel);
    RegSetValueEx(hKey,"OptimizeLevel"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    for(int i = 1; i <IC_PENSBUF; i++) {
        sprintf(key, "%s-%d", "Color", i);
        sprintf(tmpBuffer, "%d %d %d %d %.15f"/*DNT*/, pens[i].color, pens[i].number, pens[i].linetype, pens[i].speed, pens[i].weight);
        RegSetValueEx(hKey,key,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);
    }

    //registry/print/plot only - not pcp file
    sprintf(tmpBuffer, "%d", (int) area);
    RegSetValueEx(hKey,"PrintArea"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%d", (int) onlyWindowArea);
    RegSetValueEx(hKey,"OnlyWindowArea"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) +1);

    sprintf(tmpBuffer, "%f", fromX);
    RegSetValueEx(hKey,"FromX"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%f", fromY);
    RegSetValueEx(hKey,"FromY"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%f", toX);
    RegSetValueEx(hKey,"ToX"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%f", toY);
    RegSetValueEx(hKey,"ToY"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%s", view);
    RegSetValueEx(hKey,"View"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%s", header);
    RegSetValueEx(hKey,"Header"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%s", footer);
    RegSetValueEx(hKey,"Footer"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%d", (int) centerOnPage);
    RegSetValueEx(hKey,"CenterOnPage"/*DNT*/,0,REG_SZ  ,(const unsigned char *)tmpBuffer,strlen(tmpBuffer) +1);

	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
    sprintf(tmpBuffer, "%d", (int)useLWeight);
    RegSetValueEx(hKey, "UseLWeight"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);

    sprintf(tmpBuffer, "%d", (int)scaleLWeight);
    RegSetValueEx(hKey, "ScaleLWeight"/*DNT*/, 0, REG_SZ, (const unsigned char*)tmpBuffer, strlen(tmpBuffer) + 1);
	// EBATECH(CNBR) ]-

    RegCloseKey(hKey);

    return RTNORM;
}

//----------------------------------------------------------------------------
int PlotCfg::readPlotConfigFromReg()
{
    HKEY hKey;
    char key[10];
    DWORD retType;
    unsigned long nStrLen;
    char tmpBuffer[IC_PENSBUF], *loc;
    char delimiter[] = " \n"/*DNT*/;

    CString SDS_RegEntry = getPlotCfgRegistryRoot();

    if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_CURRENT_USER, SDS_RegEntry, 0,KEY_READ, &hKey)) {
        return(RTERROR);
    }

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "Version"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		version = atof(tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "Units"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		_tcscpy(units, tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "Origin"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
	{
		loc = _tcsrchr(tmpBuffer, COMMACHR);
		if (loc != NULL) {
			origin[1] = atof(loc + 1);
			*(loc) = EOSCHR;
			origin[0] = atof(tmpBuffer);
		}
	}

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "Size"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		_tcscpy(size, tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "Rotate"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		rotate = atoi(tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "Hide"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		_tcscpy(hide, tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "PenWidth"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		penwidth = atof(tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "Scale"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		_tcscpy(scale, tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "PlotFile"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		_tcscpy(plotfile, tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "FillAdjust"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		_tcscpy(filladjust, tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "OptimizeLevel"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		optimizelevel = atoi(tmpBuffer);

    for(int i = 1; i <IC_PENSBUF; i++) {
        sprintf(key, "%s-%d"/*DNT*/, "Color"/*DNT*/, i);
        nStrLen=sizeof(tmpBuffer);
        if(RegQueryValueEx(hKey, key, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		{
			pens[i].color    = atoi(_tcstok((char*)tmpBuffer, delimiter));
			pens[i].number   = atoi(_tcstok(NULL, delimiter));
			pens[i].linetype = atoi(_tcstok(NULL, delimiter));
			pens[i].speed    = atoi(_tcstok(NULL, delimiter));
			pens[i].weight   = atof(_tcstok(NULL, delimiter));
		}
    }

    //registry/print/plot only - not pcp file
    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "PrintArea"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		area = (PrintArea) atoi(tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "OnlyWindowArea"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		onlyWindowArea = atoi(tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "FromX"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		fromX = atof(tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "FromY"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		fromY = atof(tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "ToX"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		toX = atof(tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "ToY"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		toY = atof(tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "View"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		_tcscpy(view, tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "Header"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		_tcscpy(header, tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "Footer"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		_tcscpy(footer, tmpBuffer);

    nStrLen=sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "CenterOnPage"/*DNT*/,NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		centerOnPage = atoi(tmpBuffer);

	// EBATECH(CNBR) -[ 24-03-2003 Bugzilla#78471 Support Lineweight
    nStrLen = sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "UseLWeight"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		useLWeight = atoi(tmpBuffer);

    nStrLen = sizeof(tmpBuffer);
    if(RegQueryValueEx(hKey, "ScaleLWeight"/*DNT*/, NULL, &retType, (unsigned char*)tmpBuffer, &nStrLen) == ERROR_SUCCESS)
		scaleLWeight = atoi(tmpBuffer);
	// EBATECH(CNBR) ]-


    RegCloseKey(hKey);

    return RTNORM;
}
//----------------------------------------------------------------------------
BOOL PlotCfg::hasRegEntries()
{
    HKEY hKey;
    bool result = TRUE;

    CString SDS_RegEntry = getPlotCfgRegistryRoot();
    
    if(ERROR_SUCCESS!=RegOpenKeyEx(HKEY_CURRENT_USER, SDS_RegEntry, 0,KEY_READ, &hKey)) {
        result = FALSE;
    }
    RegCloseKey(hKey);

    return result;
}

//----------------------------------------------------------------------------
BOOL PlotCfg::fileExists(const char* FileName)
{
    OFSTRUCT ofstruct;
    return (!(OpenFile(FileName, &ofstruct, OF_EXIST) == HFILE_ERROR));
}


/*-------------------------------------------------------------------------*//**
The function gets system default Printer Setup dialog settings.
Code from MSDN Q246772 was used.

@author	Dmitry Gavrilov
@return 'true' on success, 'false' on failure
*//*--------------------------------------------------------------------------*/
bool PlotCfg::getSystemDefaultPrinterSettings()
{
	BOOL			bFlag;
	OSVERSIONINFO	osv;
	PRINTER_INFO_2*	pPI2 = NULL;
	DWORD			dwNeeded = 0, dwReturned = 0;

	// What version of Windows are you running?
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osv);

	// If Windows 95 or 98, use EnumPrinters...
	if(osv.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		// The first EnumPrinters() tells you how big our buffer should
		// be in order to hold ALL of PRINTER_INFO_2. Note that this will
		// usually return FALSE. This only means that the buffer (the 4th
		// parameter) was not filled in. You don't want it filled in here...
		EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, NULL, 0, &dwNeeded, &dwReturned);
		if(!dwNeeded) 
			return false;

		// Allocate enough space for PRINTER_INFO_2...
		pPI2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR, dwNeeded);
		if(!pPI2)
			return false;

		// The second EnumPrinters() will fill in all the current information...
		bFlag = EnumPrinters(PRINTER_ENUM_DEFAULT, NULL, 2, (LPBYTE)pPI2, dwNeeded, &dwNeeded, &dwReturned);
		if(!bFlag || !pPI2->pDevMode)
		{
			GlobalFree(pPI2);
			return false;
		}
	}
	else
		// If Windows NT, use the GetDefaultPrinter API for Windows 2000,
		// or GetProfileString for version 4.0 and earlier...
		if(osv.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			/*const*/ DWORD bufferSize = 250;
			TCHAR		pPrinterName[250];

#if(WINVER >= 0x0500)
			if(osv.dwMajorVersion >= 5) // Windows 2000 or later
			{
				if(!GetDefaultPrinter(pPrinterName, &bufferSize))
					return false;
			}
			else // NT4.0 or earlier
#endif
			{
				// Retrieve the default string from Win.ini (the registry).
				// String will be in form "printername,drivername,portname".
				GetProfileString("windows"/*DNT*/, "device"/*DNT*/, ",,,"/*DNT*/, pPrinterName, bufferSize);
  
				// Printer name precedes first "," character...
				if(*pPrinterName == ',' || !_tcstok(pPrinterName, ","/*DNT*/))
					return false;
			}

			HANDLE	hPrinter = NULL;
			bFlag = OpenPrinter(pPrinterName, &hPrinter, NULL);
			if(!bFlag || !hPrinter)
				return false;

			// The first GetPrinter() tells you how big our buffer should
			// be in order to hold ALL of PRINTER_INFO_2. Note that this will
			// usually return false. This only means that the buffer (the 3rd
			// parameter) was not filled in. You don't want it filled in here...
			GetPrinter(hPrinter, 2, 0, 0, &dwNeeded);
			if(!dwNeeded)
			{
				ClosePrinter(hPrinter);
				return false;
			}

			// Allocate enough space for PRINTER_INFO_2...
			if(!(pPI2 = (PRINTER_INFO_2*)GlobalAlloc(GPTR, dwNeeded)))
			{
				ClosePrinter(hPrinter);
				return false;
			}

			// The second GetPrinter() fills in all the current information...
			bFlag = GetPrinter(hPrinter, 2, (LPBYTE)pPI2, dwNeeded, &dwNeeded);
			if(!bFlag || !pPI2->pDevMode)
			{
				ClosePrinter(hPrinter);
				GlobalFree(pPI2);
				return false;
			}

			ClosePrinter(hPrinter);
		}	// platform is NT
		else
			return false;	// platform is win32s

	strcpy(m_printerName, (char*)pPI2->pDevMode->dmDeviceName);
	if(pPI2->pDevMode->dmFields & DM_ORIENTATION)
		m_paperOrientation = pPI2->pDevMode->dmOrientation;
	if(pPI2->pDevMode->dmFields & DM_PAPERSIZE)
		m_paperSize = pPI2->pDevMode->dmPaperSize;
	if(pPI2->pDevMode->dmFields & DM_PAPERLENGTH)
		m_paperLength = pPI2->pDevMode->dmPaperLength;
	if(pPI2->pDevMode->dmFields & DM_PAPERWIDTH)
		m_paperWidth = pPI2->pDevMode->dmPaperWidth;
	if(pPI2->pDevMode->dmFields & DM_DEFAULTSOURCE)
		m_paperSource = pPI2->pDevMode->dmDefaultSource;

	GlobalFree(pPI2);

	return true;
}

////////////////////////////////////////////////////////////////////////////
// Not use when integrated into IntelliCAD.
#ifdef STANDALONE
//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
//  PlotCfg pltInfo;
    
//  pltInfo.importPCP("r13.pcp");

//  pltInfo.exportPCP("mb.pcp");

//  pltInfo.exportICP("mb.icp");

//  pltInfo.importICP("mb.icp");

//  BOOL x = pltInfo.hasRegEntries();

//  pltInfo.writePlotConfigToReg();

//  x = pltInfo.hasRegEntries();

//  pltInfo.readPlotConfigFromReg();

    return RTNORM;
}
#endif // STANDALONE



