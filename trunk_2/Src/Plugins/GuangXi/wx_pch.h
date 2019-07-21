/***************************************************************
 * Name:      wx_pch.h
 * Purpose:   Header to create Pre-Compiled Header (PCH)
 * Author:    cdyw
 * Copyright: cdyw
 * License:
 **************************************************************/

#ifndef WX_PCH_H_INCLUDED
#define WX_PCH_H_INCLUDED


#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include "windows.h"
#endif


// basic wxWidgets headers
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#pragma warning( disable : 4251 )

#ifdef WX_PRECOMP
	#include <LibWRISFramework/Includes.h>
	#include <WRISFramework/Includes.h>
	#include <sdk/PluginSystem/PluginRegistrant.h>
	#include <sdk/PluginSystem/IPluginHelper.h>	
	#include <sdk/PluginSystem/IParaPageHelper.h>	

	#include "../../interface/GuangXi/GuangXi_I.h"
		
	#include "stringresource.h"
	
	#include <tinyxml++/ticpp.h>
		#include <pthread.h>
		
	#include "GuangXiMain.h"
	GuangXiPlug* GetPlug();
	
#endif // WX_PRECOMP

#endif // WX_PCH_H_INCLUDED
