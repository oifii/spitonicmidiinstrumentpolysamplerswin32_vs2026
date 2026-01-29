/*
 * Copyright (c) 2015-2016 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 3532 rue Ste-Famille, #3
 * Montreal, QC, H2X 2L1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

////////////////////////////////////////////////////////////////
//nakedsoftware.org, spi@oifii.org or stephane.poirier@oifii.org
//
//2015dec08, creation of spitonicmidiinstrumentpolysamplerswin32.cpp 
//
//2015dec08, showbytes() has not been revised, replace putchar()
//
//nakedsoftware.org, spi@oifii.org or stephane.poirier@oifii.org
////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "spitonicmidiinstrumentpolysamplerswin32.h"
#include "FreeImage.h"
#include <shellapi.h> //for CommandLineToArgW()
#include <mmsystem.h> //for timeSetEvent()
#include <stdio.h> //for swprintf()
#include <assert.h>
#include "spiwavsetlib.h"

#include "porttime.h"
#include "portmidi.h"
#include <map>

#include "portaudio.h"
#include "pa_asio.h"


#include "Tonic.h"

#include "ControlSwitcherTestSynth.h"
#include "ControlSwitcherExpSynth.h"
#include "BasicSynth.h"
#include "SimpleInstrumentSynth.h"
#include "SimpleInstrumentBufferPlayerSynth.h"
#include "SimpleInstrumentTableLookupSynth.h"
//#include "SimpleInstrumentTableLookupSPEARSynth.h"
#include "SimpleInstrumentSineSumSynth.h"
#include "SimpleInstrumentBasicSynth.h"
#include "StepSequencerSynth.h"
#include "StepSequencerExpSynth.h"
#include "StepSequencerBufferPlayerExpSynth.h"
#include "StepSequencerBufferPlayerEffectExpSynth.h"
//#include "EventsSynth.h"
#include "EventsExpSynth.h"
#include "BufferPlayerExpSynth.h"
#include "ArbitraryTableLookupSynth.h"
#include "BandlimitedOscillatorTestSynth.h"
#include "CompressorDuckingTestSynth.h"
#include "CompressorTestSynth.h"
#include "CompressorExpSynth.h"
#include "ControlSnapToScaleTestSynth.h"
#include "DelayTestSynth.h"
#include "FilteredNoiseSynth.h"
#include "FilterExpSynth.h"
#include "FMDroneSynth.h"
#include "InputDemoSynth.h"
#include "LFNoiseTestSynth.h"
#include "ReverbTestSynth.h"
#include "SimpleStepSeqSynth.h"
#include "SineSumSynth.h"
#include "StereoDelayTestSynth.h"
#include "SynthsAsGeneratorsDemoSynth.h"
#include "XYSpeedSynth.h"
#include "PolySynth.h"
using namespace Tonic;

#include "SuperBufferPlayer.h"

#include "smbPitchShift.h"

#include "spiutility.h"
#include "spimidiutility.h"

#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (224) //#define FRAMES_PER_BUFFER (512) //#define FRAMES_PER_BUFFER (2048) //#define FRAMES_PER_BUFFER (64) 
//#define NUM_CHANNELS    (1)
#define NUM_CHANNELS    (2)

#define SPITMIPS_MAXNUMBEROFSAMPLERMODULES	16

// Static smart pointer for our Synth
/*
static Synth synth;
*/
//static ControlSwitcherTestSynth synth;
//static ControlSwitcherExpSynth synth;
//static BasicSynth synth;
//static SimpleInstrumentSynth synth;
//static SimpleInstrumentBufferPlayerSynth synth;
//static SimpleInstrumentTableLookupSynth synth;
//static SimpleInstrumentTableLookupSPEARSynth synth;
static PolySynth poly[SPITMIPS_MAXNUMBEROFSAMPLERMODULES];
static Synth synth;
//static SimpleInstrumentSineSumSynth synth;
//static SimpleInstrumentBasicSynth synth;
//static StepSequencerSynth synth;
//static StepSequencerExpSynth synth;
//static StepSequencerBufferPlayerExpSynth synth;
//static StepSequencerBufferPlayerEffectExpSynth synth;
//static EventsSynth synth;
//static EventsExpSynth synth;
//static BufferPlayerExpSynth synth;
//static ArbitraryTableLookupSynth synth;
//static BandlimitedOscillatorTestSynth synth;
//static CompressorDuckingTestSynth synth;
//static CompressorTestSynth synth;
//static CompressorExpSynth synth;
//static ControlSnapToScaleTestSynth synth;
//static DelayTestSynth synth;
//static FilteredNoiseSynth synth;
//static FilterExpSynth synth;
//static FMDroneSynth synth;
//static InputDemoSynth synth;
//static LFNoiseTestSynth synth;
//static ReverbTestSynth synth;
//static SimpleStepSeqSynth synth;
//static SineSumSynth synth;
//static StereoDelayTestSynth synth;
//static SynthsAsGeneratorsDemoSynth synth;
//static XYSpeedSynth synth;



// Select sample format. 
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif

// Global Variables:

CHAR pCHAR[1024];
WCHAR pWCHAR[1024];

PmStream* global_pPmStreamMIDIIN;      // midi input 
bool global_active = false;     // set when global_pPmStreamMIDIIN is ready for reading
bool global_inited = false;     // suppress printing during command line parsing 
int global_inputmidideviceid =  11; //alesis q49 midi port id (when midi yoke installed)
std::map<string,int> global_inputmididevicemap;

//string global_instrumentnamepattern="";
string global_inputmididevicename = "Q49"; //"In From MIDI Yoke:  1", "In From MIDI Yoke:  2", ... , "In From MIDI Yoke:  8"
int global_inputmidichannel=0;
//string global_audiodevicename="E-MU ASIO"; //"Speakers (2- E-MU E-DSP Audio Processor (WDM))"
//string global_audiodevicename="Speakers (2- E-MU E-DSP Audio P"; //"E-MU ASIO"
//int global_outputAudioChannelSelectors[2]; 
std::map<string,int> global_devicemap;

//Instrument* global_pInstrument=NULL;

#define MAX_LOADSTRING 100
FIBITMAP* global_dib;
HFONT global_hFont;
HWND global_hwnd=NULL;
MMRESULT global_timer=0;
#define MAX_GLOBALTEXT	4096
WCHAR global_text[MAX_GLOBALTEXT+1];
int global_x=100;
int global_y=200;
int global_xwidth=400;
int global_yheight=400;
BYTE global_alpha=200;
int global_fontheight=24;
int global_fontwidth=-1; //will be computed within WM_PAINT handler
BYTE global_fontcolor_r=255;
BYTE global_fontcolor_g=255;
BYTE global_fontcolor_b=255;
int global_staticalignment = 0; //0 for left, 1 for center and 2 for right
int global_staticheight=-1; //will be computed within WM_SIZE handler
int global_staticwidth=-1; //will be computed within WM_SIZE handler 
//spi, begin
int global_imageheight=-1; //will be computed within WM_SIZE handler
int global_imagewidth=-1; //will be computed within WM_SIZE handler 
//spi, end
int global_titlebardisplay=1; //0 for off, 1 for on
int global_acceleratoractive=1; //0 for off, 1 for on
int global_menubardisplay=0; //0 for off, 1 for on
FILE* global_pfile=NULL;
#define IDC_MAIN_EDIT	100
#define IDC_MAIN_STATIC	101

HINSTANCE hInst;								// current instance
//TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
//TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szTitle[1024]={L"spitonicmidiinstrumentpolysamplerswin32title"};					// The title bar text
TCHAR szWindowClass[1024]={L"spitonicmidiinstrumentpolysamplerswin32class"};			// the main window class name

//new parameters
string global_begin="begin.ahk";
string global_end="end.ahk";

vector<string> global_samplefilenames;
SampleTable** global_ppbuffer[SPITMIPS_MAXNUMBEROFSAMPLERMODULES];
//BufferPlayer* global_pplayer;
const int SPITMIPS_NSAMPLES = 128; //for all the 128 midi notes
float global_sampleduration_s[SPITMIPS_MAXNUMBEROFSAMPLERMODULES][SPITMIPS_NSAMPLES];

const int SPITMIPS_NUMBEROFVOICES = 8;
SuperBufferPlayer* global_psuperplayer[SPITMIPS_MAXNUMBEROFSAMPLERMODULES];

const int SPITMIPS_MAXNUMSTAGE = 11;

string global_samplesfolder = "."; //"C:\\temp\\INSTRUMENT_SYNTH_SINWAV";
string global_samplesfilter = "*.wav";
vector<int> global_suppliedmidinotes[SPITMIPS_MAXNUMBEROFSAMPLERMODULES][SPITMIPS_MAXNUMSTAGE];
vector<int> global_pitchshiftedmidinotes[SPITMIPS_MAXNUMBEROFSAMPLERMODULES][SPITMIPS_MAXNUMSTAGE];
//#define StatusAddText StatusAddTextW

vector<string> global_samplesfolderschoice;
vector<string> global_samplesfolders;
int global_numberofsamplermodules = -1;
int global_samplermodulesindex = -1;

int voiceindex[SPITMIPS_MAXNUMBEROFSAMPLERMODULES];

//2020august28, spi, begin
string global_mididebugmode = ""; //if set to "debug" will cause the midi events to be displayed in real time in the graphic window
string global_audiomidi_devices = "disconnected";
string global_input_string = "";
int global_samplermodulesindex_selected = 0; //used when global_inputmidichannel = -3
int global_numberofoctavetoshift[SPITMIPS_MAXNUMBEROFSAMPLERMODULES];
//2020august28, spi, end

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

CSpiMidiUtility mySpiMidiUtility;


// Select sample format
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif



std::map<string, int> global_inputdevicemap;
std::map<string, int> global_outputdevicemap;

PaStream* global_stream;
PaStreamParameters global_inputParameters;
PaStreamParameters global_outputParameters;
PaError global_err;
string global_audioinputdevicename = "";
string global_audiooutputdevicename = "";
int global_inputAudioChannelSelectors[2];
int global_outputAudioChannelSelectors[2];
PaAsioStreamInfo global_asioInputInfo;
PaAsioStreamInfo global_asioOutputInfo;

FILE* pFILE = NULL;
FILE* pFILE2 = NULL;





bool global_abort = false;

static int renderCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData);

static int gNumNoInputs = 0;
// This routine will be called by the PortAudio engine when audio is needed.
// It may be called at interrupt level on some machines so don't do anything
// that could mess up the system like calling malloc() or free().
//
static int renderCallback(const void *inputBuffer, void *outputBuffer,
	unsigned long framesPerBuffer,
	const PaStreamCallbackTimeInfo* timeInfo,
	PaStreamCallbackFlags statusFlags,
	void *userData)
{
	SAMPLE *out = (SAMPLE*)outputBuffer;
	const SAMPLE *in = (const SAMPLE*)inputBuffer;
	unsigned int i;
	(void)timeInfo; // Prevent unused variable warnings.
	(void)statusFlags;
	(void)userData;

	if (global_abort == true) return paAbort;

	/*
	if( inputBuffer == NULL )
	{
	for( i=0; i<framesPerBuffer; i++ )
	{
	*out++ = 0;  // left - silent
	*out++ = 0;  // right - silent
	}
	gNumNoInputs += 1;
	}
	else
	{
	for (i = 0; i<framesPerBuffer; i++)
	{
	*out++ = *in++;  // left - unprocessed
	*out++ = *in++;  // right - unprocessed
	}
	}
	*/


	//synth.fillBufferOfFloats((float*)outputBuffer, nBufferFrames, NUM_CHANNELS);
	synth.fillBufferOfFloats((float*)outputBuffer, framesPerBuffer, NUM_CHANNELS);

	return paContinue;
}


bool SelectAudioInputDevice()
{
	const PaDeviceInfo* deviceInfo;
	int numDevices = Pa_GetDeviceCount();
	for (int i = 0; i<numDevices; i++)
	{
		deviceInfo = Pa_GetDeviceInfo(i);
		string devicenamestring = deviceInfo->name;
		global_inputdevicemap.insert(pair<string, int>(devicenamestring, i));
		if (pFILE) fprintf(pFILE, "id=%d, name=%s\n", i, devicenamestring.c_str());
	}

	int deviceid = Pa_GetDefaultInputDevice(); // default input device 
	std::map<string, int>::iterator it;
	it = global_inputdevicemap.find(global_audioinputdevicename);
	if (it != global_inputdevicemap.end())
	{
		deviceid = (*it).second;
		//printf("%s maps to %d\n", global_audiodevicename.c_str(), deviceid);
		deviceInfo = Pa_GetDeviceInfo(deviceid);
		//assert(inputAudioChannelSelectors[0]<deviceInfo->maxInputChannels);
		//assert(inputAudioChannelSelectors[1]<deviceInfo->maxInputChannels);
	}
	else
	{
		//Pa_Terminate();
		//return -1;
		//printf("error, audio device not found, will use default\n");
		//MessageBox(win,"error, audio device not found, will use default\n",0,0);
		deviceid = Pa_GetDefaultInputDevice();
	}


	global_inputParameters.device = deviceid;
	if (global_inputParameters.device == paNoDevice)
	{
		//MessageBox(win,"error, no default input device.\n",0,0);
		return false;
	}
	//global_inputParameters.channelCount = 2;
	global_inputParameters.channelCount = NUM_CHANNELS;
	global_inputParameters.sampleFormat = PA_SAMPLE_TYPE;
	global_inputParameters.suggestedLatency = Pa_GetDeviceInfo(global_inputParameters.device)->defaultLowOutputLatency;
	//inputParameters.hostApiSpecificStreamInfo = NULL;

	//Use an ASIO specific structure. WARNING - this is not portable. 
	//PaAsioStreamInfo asioInputInfo;
	global_asioInputInfo.size = sizeof(PaAsioStreamInfo);
	global_asioInputInfo.hostApiType = paASIO;
	global_asioInputInfo.version = 1;
	global_asioInputInfo.flags = paAsioUseChannelSelectors;
	global_asioInputInfo.channelSelectors = global_inputAudioChannelSelectors;
	if (deviceid == Pa_GetDefaultInputDevice())
	{
		global_inputParameters.hostApiSpecificStreamInfo = NULL;
	}
	else if (Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceid)->hostApi)->type == paASIO)
	{
		global_inputParameters.hostApiSpecificStreamInfo = &global_asioInputInfo;
	}
	else if (Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceid)->hostApi)->type == paWDMKS)
	{
		global_inputParameters.hostApiSpecificStreamInfo = NULL;
	}
	else
	{
		//assert(false);
		global_inputParameters.hostApiSpecificStreamInfo = NULL;
	}
	return true;
}



bool SelectAudioOutputDevice()
{
	const PaDeviceInfo* deviceInfo;
	int numDevices = Pa_GetDeviceCount();
	global_outputdevicemap.clear();
	for (int i = 0; i<numDevices; i++)
	{
		deviceInfo = Pa_GetDeviceInfo(i);
		string devicenamestring = deviceInfo->name;
		global_outputdevicemap.insert(pair<string, int>(devicenamestring, i));
		if (pFILE) fprintf(pFILE, "id=%d, name=%s\n", i, devicenamestring.c_str());
	}

	int deviceid = Pa_GetDefaultOutputDevice(); // default output device 
	std::map<string, int>::iterator it;
	it = global_outputdevicemap.find(global_audiooutputdevicename);
	if (it != global_outputdevicemap.end())
	{
		deviceid = (*it).second;
		//printf("%s maps to %d\n", global_audiodevicename.c_str(), deviceid);
		deviceInfo = Pa_GetDeviceInfo(deviceid);
		//assert(inputAudioChannelSelectors[0]<deviceInfo->maxInputChannels);
		//assert(inputAudioChannelSelectors[1]<deviceInfo->maxInputChannels);
	}
	else
	{
		//Pa_Terminate();
		//return -1;
		//printf("error, audio device not found, will use default\n");
		//MessageBox(win,"error, audio device not found, will use default\n",0,0);
		deviceid = Pa_GetDefaultOutputDevice();
	}


	global_outputParameters.device = deviceid;
	if (global_outputParameters.device == paNoDevice)
	{
		//MessageBox(win,"error, no default output device.\n",0,0);
		return false;
	}
	//global_inputParameters.channelCount = 2;
	global_outputParameters.channelCount = NUM_CHANNELS;
	global_outputParameters.sampleFormat = PA_SAMPLE_TYPE;
	global_outputParameters.suggestedLatency = Pa_GetDeviceInfo(global_outputParameters.device)->defaultLowOutputLatency;
	//outputParameters.hostApiSpecificStreamInfo = NULL;

	//Use an ASIO specific structure. WARNING - this is not portable. 
	//PaAsioStreamInfo asioInputInfo;
	global_asioOutputInfo.size = sizeof(PaAsioStreamInfo);
	global_asioOutputInfo.hostApiType = paASIO;
	global_asioOutputInfo.version = 1;
	global_asioOutputInfo.flags = paAsioUseChannelSelectors;
	global_asioOutputInfo.channelSelectors = global_outputAudioChannelSelectors;
	if (deviceid == Pa_GetDefaultOutputDevice())
	{
		global_outputParameters.hostApiSpecificStreamInfo = NULL;
	}
	else if (Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceid)->hostApi)->type == paASIO)
	{
		global_outputParameters.hostApiSpecificStreamInfo = &global_asioOutputInfo;
	}
	else if (Pa_GetHostApiInfo(Pa_GetDeviceInfo(deviceid)->hostApi)->type == paWDMKS)
	{
		global_outputParameters.hostApiSpecificStreamInfo = NULL;
	}
	else
	{
		//assert(false);
		global_outputParameters.hostApiSpecificStreamInfo = NULL;
	}
	return true;
}


void receive_poll(PtTimestamp timestamp, void *userData)
{
    PmEvent event;
    int count; 
    if (!global_active) return;
    while ((count = Pm_Read(global_pPmStreamMIDIIN, &event, 1))) 
	{
        if (count == 1) 
		{
			//0) detect channel
			int command = Pm_MessageStatus(event.message) & MIDI_CODE_MASK;
			int chan = Pm_MessageStatus(event.message) & MIDI_CHN_MASK;
			int data1 = Pm_MessageData1(event.message);
			int data2 = Pm_MessageData2(event.message);
			if ( (global_inputmidichannel>-1) && (chan == global_inputmidichannel) )
			{
				//chan is the midi channel id, between 0 and 15
				//in this case, the incoming channel is mapped to the selected sampler module
				chan = global_samplermodulesindex_selected;
				//1) output message
				if (global_mididebugmode=="debug") mySpiMidiUtility.output(event.message);

				//2) 
				if (command == MIDI_OFF_NOTE || (command == MIDI_ON_NOTE && data2==0))
				{
					int midinotenumber = data1; //range 0 to 127
					midinotenumber += (12 * global_numberofoctavetoshift[chan]);
					if (midinotenumber<0 || midinotenumber>127) midinotenumber = data1;
					int midinotevelocity = data2; //range 0 to 127
					poly[chan].noteOff(midinotenumber);
				}
				else if (command == MIDI_ON_NOTE)
				{
					int midinotenumber = data1; //range 0 to 127
					midinotenumber += (12 * global_numberofoctavetoshift[chan]);
					if (midinotenumber<0 || midinotenumber>127) midinotenumber = data1;
					int midinotevelocity = data2; //range 0 to 127
					poly[chan].noteOn(chan, midinotenumber, midinotevelocity);
				}
				else if (command == MIDI_CH_PROGRAM)
				{

				}
				else if (command == MIDI_CTRL)
				{
					if(data1==0 && data2==0)
					{
						/*
						//output(event.message);
						ShellExecuteA(NULL, "open", ".\\begin.ahk", "", NULL, false);
						*/
					}
				}
			}
			else if ( (global_inputmidichannel == -1) && (chan < global_numberofsamplermodules) )
			{
				//1) output message
				if (global_mididebugmode == "debug") mySpiMidiUtility.output(event.message);

				//2) 
				if (command == MIDI_OFF_NOTE || (command == MIDI_ON_NOTE && data2 == 0))
				{
					int midinotenumber = data1; //range 0 to 127
					midinotenumber += (12 * global_numberofoctavetoshift[chan]);
					if (midinotenumber<0 || midinotenumber>127) midinotenumber = data1;
					int midinotevelocity = data2; //range 0 to 127
					poly[chan].noteOff(midinotenumber);
				}
				else if (command == MIDI_ON_NOTE)
				{
					int midinotenumber = data1; //range 0 to 127
					midinotenumber += (12 * global_numberofoctavetoshift[chan]);
					if (midinotenumber<0 || midinotenumber>127) midinotenumber = data1;
					int midinotevelocity = data2; //range 0 to 127
					poly[chan].noteOn(chan, midinotenumber, midinotevelocity);
				}
				else if (command == MIDI_CH_PROGRAM)
				{

				}
				else if (command == MIDI_CTRL)
				{
					if (data1 == 0 && data2 == 0)
					{
						/*
						//output(event.message);
						ShellExecuteA(NULL, "open", ".\\begin.ahk", "", NULL, false);
						*/
					}
				}
			}
			else if (global_inputmidichannel == -2)
			{
				//chan is the midi channel id, between 0 and 15
				if (chan < global_numberofsamplermodules)
				{
					//nothing extra to be done
				}
				else
				{
					//must map the midi channel id to one of the sampler modules
					chan = chan % global_numberofsamplermodules;
				}
				//1) output message
				if (global_mididebugmode == "debug") mySpiMidiUtility.output(event.message);

				//2) 
				if (command == MIDI_OFF_NOTE || (command == MIDI_ON_NOTE && data2 == 0))
				{
					int midinotenumber = data1; //range 0 to 127
					midinotenumber += (12 * global_numberofoctavetoshift[chan]);
					if (midinotenumber<0 || midinotenumber>127) midinotenumber = data1;
					int midinotevelocity = data2; //range 0 to 127
					poly[chan].noteOff(midinotenumber);
				}
				else if (command == MIDI_ON_NOTE)
				{
					int midinotenumber = data1; //range 0 to 127
					midinotenumber += (12 * global_numberofoctavetoshift[chan]);
					if (midinotenumber<0 || midinotenumber>127) midinotenumber = data1;
					int midinotevelocity = data2; //range 0 to 127
					poly[chan].noteOn(chan, midinotenumber, midinotevelocity);
				}
				else if (command == MIDI_CH_PROGRAM)
				{

				}
				else if (command == MIDI_CTRL)
				{
					if (data1 == 0 && data2 == 0)
					{
						/*
						//output(event.message);
						ShellExecuteA(NULL, "open", ".\\begin.ahk", "", NULL, false);
						*/
					}
				}
			}
			else if (global_inputmidichannel == -3)
			{
				//chan is the midi channel id, between 0 and 15
				//in this case, all incoming channels are mapped to the selected sampler module
				chan = global_samplermodulesindex_selected;
				//1) output message
				if (global_mididebugmode == "debug") mySpiMidiUtility.output(event.message);

				//2) 
				if (command == MIDI_OFF_NOTE || (command == MIDI_ON_NOTE && data2 == 0))
				{
					int midinotenumber = data1; //range 0 to 127
					midinotenumber += (12 * global_numberofoctavetoshift[chan]);
					if (midinotenumber<0 || midinotenumber>127) midinotenumber = data1;
					int midinotevelocity = data2; //range 0 to 127
					poly[chan].noteOff(midinotenumber);
				}
				else if (command == MIDI_ON_NOTE)
				{
					int midinotenumber = data1; //range 0 to 127
					midinotenumber += (12 * global_numberofoctavetoshift[chan]);
					if (midinotenumber<0 || midinotenumber>127) midinotenumber = data1;
					int midinotevelocity = data2; //range 0 to 127
					poly[chan].noteOn(chan, midinotenumber, midinotevelocity);
				}
				else if (command == MIDI_CH_PROGRAM)
				{

				}
				else if (command == MIDI_CTRL)
				{
					if (data1 == 0 && data2 == 0)
					{
						/*
						//output(event.message);
						ShellExecuteA(NULL, "open", ".\\begin.ahk", "", NULL, false);
						*/
					}
				}
			}
		}
        else            
		{
			//printf(Pm_GetErrorText((PmError)count)); //spi a cast as (PmError)
			sprintf(pCHAR, Pm_GetErrorText((PmError)count));StatusAddTextA(pCHAR);
		}
    }
}

void CALLBACK StartGlobalProcess(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	//WavSetLib_Initialize(global_hwnd, IDC_MAIN_STATIC, global_staticwidth, global_staticheight, global_fontwidth, global_fontheight);
	//global_pfile = fopen("output.txt", "w");
	global_pfile = NULL;
	WavSetLib_Initialize(global_hwnd, IDC_MAIN_STATIC, global_staticwidth, global_staticheight, global_fontwidth, global_fontheight, global_staticalignment, global_pfile);

	//testing start /b, it does not work, like if /b has not effect
	//system("start /b c:\\app-bin\\sox\\sox.exe -q \"d:\\temp\\test.wav\" -d trim 0 10.0");
	//testing ShellExecute(), it works
	//ShellExecute(NULL, L"open", L"c:\\app-bin\\sox\\sox.exe", L"-q \"d:\\temp\\test.wav\" -d trim 0 10.0", NULL, 0);



	/////////////////////
	//initialize portmidi
	/////////////////////
    PmError err;
	err = Pm_Initialize(); 
	if (err)
	{
		sprintf(pCHAR, Pm_GetErrorText(err)); StatusAddTextA(pCHAR);
		if (pFILE) fprintf(pFILE, "error,  Pm_Initialize() failed returning error %s\n", pCHAR);
		if (pFILE) fflush(pFILE);
		//Pt_Stop();
		//Terminate();
		//mmexit(1);
		return;
	}

	/////////////////////////////
	//input midi device selection
	/////////////////////////////
	const PmDeviceInfo* deviceInfo;
    int numDevices = Pm_CountDevices();
	global_inputmididevicemap.clear();
    for( int i=0; i<numDevices; i++ )
    {
        deviceInfo = Pm_GetDeviceInfo( i );
		if (deviceInfo->input)
		{
			string devicenamestring = deviceInfo->name;
			global_inputmididevicemap.insert(pair<string,int>(devicenamestring,i));
		}
	}
	std::map<string,int>::iterator it;
	it = global_inputmididevicemap.find(global_inputmididevicename);
	if(it!=global_inputmididevicemap.end())
	{
		global_inputmidideviceid = (*it).second;
		sprintf(pCHAR, "%s maps to %d\n", global_inputmididevicename.c_str(), global_inputmidideviceid);StatusAddTextA(pCHAR); //spi note: crashes on first VS2013 compile
		if (pFILE) fprintf(pFILE, "found input midi device %s\n", pCHAR);
		if (pFILE) fflush(pFILE);
		deviceInfo = Pm_GetDeviceInfo(global_inputmidideviceid);
	}
	else
	{
		assert(false);
		for(it=global_inputmididevicemap.begin(); it!=global_inputmididevicemap.end(); it++)
		{
			sprintf(pCHAR, "%s maps to %d\n", (*it).first.c_str(), (*it).second);StatusAddTextA(pCHAR);
		}
		swprintf(pWCHAR, L"input midi device not found\n");StatusAddText(pWCHAR);
		if (pFILE) fwprintf(pFILE, L"%s\n", pWCHAR);
		if (pFILE) fflush(pFILE);
		return;
	}

    // use porttime callback to empty midi queue and print 
	PtError pterr = Pt_Start(1, receive_poll, 0); //Pt_Start(1, receive_poll, global_pInstrument); 
	if (pterr)
	{
		if (pFILE) fprintf(pFILE, "error,  Pt_Start() failed returning error %d\n", pterr);
		if (pFILE) fflush(pFILE);
		//Pt_Stop();
		//Terminate();
		//mmexit(1);
		return;
	}

    // list device information 
    swprintf(pWCHAR, L"MIDI input devices:\n");StatusAddText(pWCHAR);
	if (pFILE) fwprintf(pFILE, L"%s\n", pWCHAR);
    for (int i = 0; i < Pm_CountDevices(); i++) 
	{
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
        if (info->input) 
		{
			sprintf(pCHAR, "%d: %s, %s\n", i, info->interf, info->name);StatusAddTextA(pCHAR);
			if (pFILE) fprintf(pFILE, "%s\n", pCHAR);
		}
    }
    //inputmididevice = get_number("Type input device number: ");
	swprintf(pWCHAR, L"device %d selected\n", global_inputmidideviceid);StatusAddText(pWCHAR);
	if (pFILE) fwprintf(pFILE, L"%s\n", pWCHAR);
	if (pFILE) fflush(pFILE);
    err = Pm_OpenInput(&global_pPmStreamMIDIIN, global_inputmidideviceid, NULL, 512, NULL, NULL);
    if (err) 
	{
        sprintf(pCHAR, Pm_GetErrorText(err));StatusAddTextA(pCHAR);
        Pt_Stop();
		//Terminate();
        //mmexit(1);
		return;
    }
	Pm_SetFilter(global_pPmStreamMIDIIN, mySpiMidiUtility.filter);
    global_inited = true; // now can document changes, set filter 
    swprintf(pWCHAR, L"spitonicmidiinstrumentpolysamplerswin32 ready.\n");StatusAddText(pWCHAR);
    global_active = true;
	global_audiomidi_devices = "connected";

	/*
	//1) load a sample file
	SndfileHandle file1;
	//file1 = SndfileHandle("D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\worldaudio_wav\\00min15sec-and-less\\Geoffrey Oryema - TAO -  mara(introlater)_9sec.wav");
	file1 = SndfileHandle("D:\\oifii-org\\httpdocs\\ha-org\\had\\dj-oifii\\worldaudio_wav\\00min30sec-and-less\\GF - Subramanian - track 03(intro)_18sec.wav");
	assert(file1.samplerate() == 44100);
	assert(file1.channels() == 2);
	float file1duration_s = ((float)file1.frames()) / ((float)file1.samplerate());
	while (0)
	{
		//terminate portmidi
		//global_active = false;
		//Pm_Close(global_pPmStreamMIDIIN);
		//Pt_Stop();
		//Pm_Terminate();

		//play synth automatically
		int random_integer;
		int lowest = 36, highest = 84;
		int range = (highest - lowest) + 1;
		random_integer = lowest + int(range*rand() / (RAND_MAX + 1.0));
		//int midinotenumber = 64; //range 0 to 127
		int midinotenumber = random_integer; //range 0 to 127
		//int midinotevelocity = 127; //range 0 to 127
		lowest = 10; highest = 127;
		random_integer = lowest + int(range*rand() / (RAND_MAX + 1.0));
		int midinotevelocity = random_integer; //range 0 to 127
		if (midinotevelocity != 0)
		{
			swprintf(pWCHAR, L"auto Note %d , Velocity %d\n", midinotenumber, midinotevelocity); StatusAddText(pWCHAR);

			//2.1) set a parameter that we created when we defined the synth
			synth.setParameter("midiNote", midinotenumber);
			synth.setParameter("midiNoteVelocity", midinotevelocity);
			//2.2) trigger note
			//simply setting the value of a parameter causes that parameter to send a "trigger" message to any using them as triggers
			synth.setParameter("trigger", 1);
		}


		Sleep(file1duration_s * 1000);
	}
	//PostMessage(global_hwnd, WM_DESTROY, 0, 0);
	*/
}


//WavSet myWavSet;
void pitchshift(int midinote, int referencemidinote)
{
	int semitones = midinote - referencemidinote; //semitone shift: -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1 or 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
	assert(semitones > -13 && semitones < 13);
	///////////////////////////////////////
	//create wavset from tonic sample table
	///////////////////////////////////////
	WavSet myWavSet(44100, 
		global_ppbuffer[global_samplermodulesindex][referencemidinote]->channels(),
		global_ppbuffer[global_samplermodulesindex][referencemidinote]->frames(),
		(float*)(global_ppbuffer[global_samplermodulesindex][referencemidinote]->dataPointer()));
	/*
	if (midinote == 83 && referencemidinote == 71)
	{
		myWavSet.Play(&global_outputParameters);
	}
	*/

	///////////////////////////////
	//split left and right channels
	///////////////////////////////
	WavSet myLeftWavSet;
	WavSet myRightWavSet;
	myWavSet.GetLeftChannel(&myLeftWavSet);
	myWavSet.GetRightChannel(&myRightWavSet);

	/*
	if (midinote == 83 && referencemidinote == 71)
	{
		//myWavSet.Play(&global_outputParameters);
		//myLeftWavSet.Play(&global_outputParameters);
		//myRightWavSet.Play(&global_outputParameters);

		//myWavSet.Play(USING_SPIPLAYX, 1.0);
		//myLeftWavSet.Play(USING_SPIPLAYX, 1.0);
		//myRightWavSet.Play(USING_SPIPLAYX, 1.0);
	}
	*/

	/////////////////////
	//perform pitch shift
	/////////////////////
	//semitones = 3;	// shift up by 3 semitones
	//semitones = -3; // shift down by 3 semitones
	float pitchShift = pow(2., semitones / 12.);	// convert semitones to factor
	smbPitchShift(pitchShift, myLeftWavSet.numSamples, 2048, 4, 44100.0, myLeftWavSet.pSamples, myLeftWavSet.pSamples);
	smbPitchShift(pitchShift, myRightWavSet.numSamples, 2048, 4, 44100.0, myRightWavSet.pSamples, myRightWavSet.pSamples);

	//recombine left and right channels
	WavSet myPitchShiftedWavSet;
	myPitchShiftedWavSet.SetLeftAndRightChannels(&myLeftWavSet, &myRightWavSet);

	/*
	if (midinote == 83 && referencemidinote == 71)
	{
		//myLeftWavSet.Play(&global_outputParameters);
		//myRightWavSet.Play(&global_outputParameters);
		//myPitchShiftedWavSet.Play(&global_outputParameters);

		//myLeftWavSet.Play(USING_SPIPLAYX, 1.0);
		//myRightWavSet.Play(USING_SPIPLAYX, 1.0);
		//myPitchShiftedWavSet.Play(USING_SPIPLAYX, 1.0);
	}
	*/

	//create tonic sample table
	assert(global_ppbuffer[global_samplermodulesindex][midinote] == NULL);
	global_sampleduration_s[global_samplermodulesindex][midinote] = ((float)myPitchShiftedWavSet.totalFrames) / ((float)myPitchShiftedWavSet.SampleRate);
	global_ppbuffer[global_samplermodulesindex][midinote] = new SampleTable(myPitchShiftedWavSet.totalFrames, myPitchShiftedWavSet.numChannels);
	memcpy(global_ppbuffer[global_samplermodulesindex][midinote]->dataPointer(), myPitchShiftedWavSet.pSamples, myPitchShiftedWavSet.totalFrames*myPitchShiftedWavSet.numChannels*sizeof(float));
	
	//write to file - for debugging purpose
	if (pFILE2)
	{
		fprintf(pFILE2, "pitchshifted midinote %d from referencemidinote %d\n", midinote, referencemidinote);
	}
}

bool hasmidinotegaps()
{
	bool bgapfound = false;
	for (int midinote = 0; midinote < SPITMIPS_NSAMPLES; midinote++)
	{
		if (global_ppbuffer[global_samplermodulesindex][midinote] == NULL)
		{
			bgapfound = true;
			break;
		}
	}
	return bgapfound;
}

void loadSynthSamples(string samplesfolder, string samplesfilter)
{
	//global_samplermodulesindex

	///////////////////////////
	//populate sample filenames
	///////////////////////////
	if (samplesfolder != "" && samplesfilter != "")
	{
		//1) execute cmd line to get all folder's image filenames
		string quote = "\"";
		string pathfilter;
		string path = samplesfolder;
		//pathfilter = path + "\\*.bmp";
		pathfilter = path + "\\" + samplesfilter;
		string systemcommand;
		//systemcommand = "DIR " + quote + pathfilter + quote + "/B /O:N > wsic_filenames.txt"; //wsip tag standing for wav set (library) instrumentset (class) populate (function)
		systemcommand = "DIR " + quote + pathfilter + quote + "/B /S /O:N > spitmips_filenames.txt"; // /S for adding path into "spiwtmvs_filenames.txt"
		system(systemcommand.c_str());
		//2) load in all "spiwtmvs_filenames.txt" file
		//Sleep(1000);
		//vector<string> global_imagefilenames;
		ifstream ifs("spitmips_filenames.txt");
		string temp;
		global_samplefilenames.clear();
		while (getline(ifs, temp))
		{
			//txtfilenames.push_back(path + "\\" + temp);
			global_samplefilenames.push_back(temp);
		}
	}
	
	global_ppbuffer[global_samplermodulesindex] = new SampleTable*[SPITMIPS_NSAMPLES];
	for (int i = 0; i < SPITMIPS_NSAMPLES; i++)
	{
		global_ppbuffer[global_samplermodulesindex][i] = NULL;
	}
	for (int i = 0; i < global_samplefilenames.size(); i++)
	{
		WavSet myWavSet;
		myWavSet.ReadWavFile(global_samplefilenames[i].c_str());
		if (myWavSet.SampleRate == 48000)
		{
			myWavSet.SampleRate = 44100; //it will shift the frequency, but hey!
			if (pFILE2)
			{
				fprintf(pFILE2, "warning, samplerate forced to 44100, samplerate was 48000 for sample name %s\n", global_samplefilenames[i].c_str());
			}
		}
		if (myWavSet.SampleRate != 44100)
		{
			if (pFILE2)
			{
				fprintf(pFILE2, "error, samplerate different than 44100, samplerate is %d for sample name %s\n", myWavSet.SampleRate, global_samplefilenames[i].c_str());
				fclose(pFILE2);
			}
			exit(1);
		}
		if (myWavSet.numChannels == 1)
		{
			myWavSet.Resample44100monoTo44100stereo();
		}
		if (myWavSet.numChannels != 2)
		{
			if (pFILE2)
			{
				fprintf(pFILE2, "error, channels different than 2, channels is %d for sample name %s\n", myWavSet.numChannels, global_samplefilenames[i].c_str());
				fclose(pFILE2);
			}
			exit(1);
		}


		int midinote = GetMidiNoteNumberFromString(global_samplefilenames[i].c_str());
		if (midinote<0 || midinote>(SPITMIPS_NSAMPLES - 1))
		{
			if (pFILE2)
			{
				fprintf(pFILE2, "error, midinote %d unknown for sample name %s\n", midinote, global_samplefilenames[i].c_str());
				fclose(pFILE2);
			}
			exit(1);
		}
		else
		{
			if (pFILE2)
			{
				fprintf(pFILE2, "success, found midinote %d for sample name %s\n", midinote, global_samplefilenames[i].c_str());
			}
		}
		if (global_ppbuffer[global_samplermodulesindex][midinote] == NULL)
		{
			global_sampleduration_s[global_samplermodulesindex][midinote] = ((float)myWavSet.totalFrames) / ((float)myWavSet.SampleRate);
			global_ppbuffer[global_samplermodulesindex][midinote] = new SampleTable(myWavSet.totalFrames, myWavSet.numChannels);
			memcpy(global_ppbuffer[global_samplermodulesindex][midinote]->dataPointer(), myWavSet.pSamples, myWavSet.totalFrames*myWavSet.numChannels*sizeof(float));
			global_suppliedmidinotes[global_samplermodulesindex][0].push_back(midinote);
		}
		else
		{
			if (pFILE2)
			{
				fprintf(pFILE2, "warning, detected more than one sample for midinote %d\n", midinote);
			}
		}

	}
	if (pFILE2)
	{
		fflush(pFILE2);
	}
	
	int stage = -1;
	while (hasmidinotegaps() == true && stage<(SPITMIPS_MAXNUMSTAGE-1))
	{
		stage++;

		///////////////////////////////////////////////////
		//pitch shift nearest samples to fill midinote gaps
		///////////////////////////////////////////////////

		std::sort(global_suppliedmidinotes[global_samplermodulesindex][stage].begin(), global_suppliedmidinotes[global_samplermodulesindex][stage].end());

		for (int i = 0; i < global_suppliedmidinotes[global_samplermodulesindex][stage].size() + 1; i++)
		{
			if (i == 0 && global_suppliedmidinotes[global_samplermodulesindex][stage][i]>0)
			{
				///////////////////////////////////
				//between first supplied note and 0
				///////////////////////////////////
				for (int midinote = global_suppliedmidinotes[global_samplermodulesindex][stage][i] - 1; midinote > -1; midinote--)
				{
					int semitones = midinote - global_suppliedmidinotes[global_samplermodulesindex][stage][i]; //semitone shift: -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1 or 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
					if (semitones > -13 && semitones < 13)
					{
						global_pitchshiftedmidinotes[global_samplermodulesindex][stage].push_back(midinote);
						pitchshift(midinote, global_suppliedmidinotes[global_samplermodulesindex][stage][i]);
					}
					else
					{
						assert(global_ppbuffer[global_samplermodulesindex][midinote] == NULL);
					}

				}
			}
			else if (i == global_suppliedmidinotes[global_samplermodulesindex][stage].size() && global_suppliedmidinotes[global_samplermodulesindex][stage][i - 1]<127)
			{
				////////////////////////////////////
				//between last supplied note and 127
				////////////////////////////////////
				for (int midinote = global_suppliedmidinotes[global_samplermodulesindex][stage][i - 1] + 1; midinote < 128; midinote++)
				{
					int semitones = midinote - global_suppliedmidinotes[global_samplermodulesindex][stage][i - 1]; //semitone shift: -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1 or 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
					if (semitones > -13 && semitones < 13)
					{
						global_pitchshiftedmidinotes[global_samplermodulesindex][stage].push_back(midinote);
						pitchshift(midinote, global_suppliedmidinotes[global_samplermodulesindex][stage][i - 1]);
					}
					else
					{
						assert(global_ppbuffer[global_samplermodulesindex][midinote] == NULL);
					}
				}
			}
			else if (i>0 && i<global_suppliedmidinotes[global_samplermodulesindex][stage].size())
			//else
			{
				///////////////////////////
				//in between supplied notes
				///////////////////////////
				for (int midinote = global_suppliedmidinotes[global_samplermodulesindex][stage][i] - 1; midinote > global_suppliedmidinotes[global_samplermodulesindex][stage][i - 1]; midinote--)
				{
					int semitones_a = midinote - global_suppliedmidinotes[global_samplermodulesindex][stage][i]; //semitone shift: -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1 or 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
					int semitones_b = midinote - global_suppliedmidinotes[global_samplermodulesindex][stage][i - 1]; //semitone shift: -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1 or 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12

					if ((abs(semitones_a) <= abs(semitones_b)) && (semitones_a > -13 && semitones_a < 13))
					{
						global_pitchshiftedmidinotes[global_samplermodulesindex][stage].push_back(midinote);
						pitchshift(midinote, global_suppliedmidinotes[global_samplermodulesindex][stage][i]);
					}
					else if ((abs(semitones_b)<abs(semitones_a)) && (semitones_b > -13 && semitones_b < 13))
					{
						global_pitchshiftedmidinotes[global_samplermodulesindex][stage].push_back(midinote);
						pitchshift(midinote, global_suppliedmidinotes[global_samplermodulesindex][stage][i - 1]);
					}
					else
					{
						assert(global_ppbuffer[global_samplermodulesindex][midinote] == NULL);
					}
				}
			}
		}

		if ((stage + 1) < SPITMIPS_MAXNUMSTAGE)
		{
			global_suppliedmidinotes[global_samplermodulesindex][stage + 1] = global_suppliedmidinotes[global_samplermodulesindex][stage];
			global_suppliedmidinotes[global_samplermodulesindex][stage + 1].insert(global_suppliedmidinotes[global_samplermodulesindex][stage + 1].end(), global_pitchshiftedmidinotes[global_samplermodulesindex][stage].begin(), global_pitchshiftedmidinotes[global_samplermodulesindex][stage].end());
		}
	}
	
	bool berrorfound = false;
	for (int midinote = 0; midinote < SPITMIPS_NSAMPLES; midinote++)
	{
		if (global_ppbuffer[global_samplermodulesindex][midinote] == NULL)
		{
			SndfileHandle file3;
			file3 = SndfileHandle("silence-stereo_10sec.wav");
			assert(file3.samplerate() == 44100);
			assert(file3.channels() == 2);
			assert(global_ppbuffer[global_samplermodulesindex][midinote] == NULL);
			global_sampleduration_s[global_samplermodulesindex][midinote] = ((float)file3.frames()) / ((float)file3.samplerate());
			global_ppbuffer[global_samplermodulesindex][midinote] = new SampleTable(file3.frames(), file3.channels());
			file3.read(global_ppbuffer[global_samplermodulesindex][midinote]->dataPointer(), file3.frames()*file3.channels());
			if (pFILE2)
			{
				fprintf(pFILE2, "warning, sample silence for midinote %d\n", midinote);
			}

		}
	}
	if (berrorfound)
	{
		if (pFILE2)
		{
			fclose(pFILE2);
		}
		exit(1);
	}
	else if (pFILE2)
	{
		fflush(pFILE2);
	}


	global_psuperplayer[global_samplermodulesindex] = new SuperBufferPlayer[SPITMIPS_NUMBEROFVOICES];
	for (int i = 0; i < SPITMIPS_NUMBEROFVOICES; i++)
	{
		global_psuperplayer[global_samplermodulesindex][i].setBuffers(global_ppbuffer[global_samplermodulesindex]);
	}
	return;
}

void unloadSynthSamples()
{
	for (int i = 0; i < SPITMIPS_NSAMPLES; i++)
	{
		delete global_ppbuffer[global_samplermodulesindex][i];
	}
	delete[] global_ppbuffer[global_samplermodulesindex];
	//delete[] global_pplayer;
	delete[] global_psuperplayer[global_samplermodulesindex];
}

//int voiceindex = -1;
Synth createSynthVoice()
{
	voiceindex[global_samplermodulesindex]++;
	Synth newSynth;

	ControlParameter noteNum = newSynth.addParameter("polyNote", 0.0);
	ControlParameter gate = newSynth.addParameter("polyGate", 0.0);
	ControlParameter noteVelocity = newSynth.addParameter("polyVelocity", 0.0);
	ControlParameter voiceNumber = newSynth.addParameter("polyVoiceNumber", 0.0);

	ControlGenerator voiceFreq = ControlMidiToFreq().input(noteNum) + voiceNumber * 1.2; // detune the voices slightly
	//ControlGenerator midinoteNum = noteNum;

	//Generator tone = SquareWave().freq(voiceFreq) * SineWave().freq(50);
	
	//global_bufferplayers = ControlSwitcher().inputIndex(noteNum);

	//Generator tone = global_pplayer[0].trigger(gate);
	Generator tone = global_psuperplayer[global_samplermodulesindex][voiceindex[global_samplermodulesindex]].setBuffer(noteNum).trigger(gate);

	//Generator tone = global_bufferplayers.inputIndex(noteNum).trigger(gate);

	ADSR env = ADSR()
		.attack(0.04)
		.decay(0.1)
		//.decay(global_sampleduration_s[0])
		//.decay(4.0)
		
		.sustain(0.8)
		.release(0.0)
		.doesSustain(true)
		
		/*
		.sustain(0)
		.release(0)
		.doesSustain(false)
		*/
		.trigger(gate);

	ControlGenerator filterFreq = voiceFreq * 0.5 + 200;

	//LPF24 filter = LPF24().Q(1.0 + noteVelocity * 0.02).cutoff(filterFreq); //original
	LPF24 filter = LPF24().cutoff(filterFreq); //spi

	//Generator output = ((tone * env) >> filter) * (0.02 + noteVelocity * 0.005); //original
	//Generator output = ((tone * env) >> filter);
	Generator output = (tone * env);

	newSynth.setOutputGen(output);

	return newSynth;
}


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//LPWSTR *szArgList;
	LPSTR *szArgList;
	int nArgs;
	int i;

	//szArgList = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	szArgList = CommandLineToArgvA(GetCommandLineA(), &nArgs);
	if( NULL == szArgList )
	{
		//wprintf(L"CommandLineToArgvW failed\n");
		return FALSE;
	}
	LPWSTR *szArgListW;
	int nArgsW;
	szArgListW = CommandLineToArgvW(GetCommandLineW(), &nArgsW);
	if( NULL == szArgListW )
	{
		//wprintf(L"CommandLineToArgvW failed\n");
		return FALSE;
	}

	if(nArgs>1)
	{
		//inputmididevice=atoi(argv[2]);
		global_inputmididevicename=szArgList[1]; //"Q49", "In From MIDI Yoke:  1", "In From MIDI Yoke:  2", ... , "In From MIDI Yoke:  8"
	}
	//int inputmididevice =  11; //alesis q49 midi port id (when midi yoke installed)
	//int inputmididevice =  1; //midi yoke 1 (when midi yoke installed)
	if(nArgs>2)
	{
		global_inputmidichannel=atoi(szArgList[2]); //midi channel id to listen to, between 0 and 15, 
													//if -1 will listen to all midi channels for which a sampler module exists,
													//if -2 will listen to all midi channels mapping all notes to existing sampler modules
													//if -3 will listen to all midi channels mapping all notes to an existing sampler module randomly selected
													//      that can be toggled by left mouse down button at runtime 
	}

	global_audiooutputdevicename = "E-MU ASIO"; //"Wave (2- E-MU E-DSP Audio Proce"
	if (nArgs>3)
	{
		//global_filename = szArgList[1];
		global_audiooutputdevicename = szArgList[3];
	}
	global_outputAudioChannelSelectors[0] = 0; // on emu patchmix ASIO device channel 1 (left)
	global_outputAudioChannelSelectors[1] = 1; // on emu patchmix ASIO device channel 2 (right)
	//global_outputAudioChannelSelectors[0] = 2; // on emu patchmix ASIO device channel 3 (left)
	//global_outputAudioChannelSelectors[1] = 3; // on emu patchmix ASIO device channel 4 (right)
	//global_outputAudioChannelSelectors[0] = 8; // on emu patchmix ASIO device channel 9 (left)
	//global_outputAudioChannelSelectors[1] = 9; // on emu patchmix ASIO device channel 10 (right)
	//global_outputAudioChannelSelectors[0] = 10; // on emu patchmix ASIO device channel 11 (left)
	//global_outputAudioChannelSelectors[1] = 11; // on emu patchmix ASIO device channel 12 (right)
	if (nArgs>4)
	{
		global_outputAudioChannelSelectors[0] = atoi((LPCSTR)(szArgList[4])); //0 for first asio channel (left) or 2, 4, 6, etc.
	}
	if (nArgs>5)
	{
		global_outputAudioChannelSelectors[1] = atoi((LPCSTR)(szArgList[5])); //1 for second asio channel (right) or 3, 5, 7, etc.
	}

	if(nArgs>6)
	{
		global_x = atoi(szArgList[6]);
	}
	if(nArgs>7)
	{
		global_y = atoi(szArgList[7]);
	}
	if(nArgs>8)
	{
		global_xwidth = atoi(szArgList[8]);
	}
	if(nArgs>9)
	{
		global_yheight = atoi(szArgList[9]);
	}
	if(nArgs>10)
	{
		global_alpha = atoi(szArgList[10]);
	}
	if(nArgs>11)
	{
		global_titlebardisplay = atoi(szArgList[11]);
	}
	if(nArgs>12)
	{
		global_menubardisplay = atoi(szArgList[12]);
	}
	if(nArgs>13)
	{
		global_acceleratoractive = atoi(szArgList[13]);
	}
	if(nArgs>14)
	{
		global_fontheight = atoi(szArgList[14]);
	}
	if(nArgs>15)
	{
		global_fontcolor_r = atoi(szArgList[15]);
	}
	if(nArgs>16)
	{
		global_fontcolor_g = atoi(szArgList[16]);
	}
	if(nArgs>17)
	{
		global_fontcolor_b = atoi(szArgList[17]);
	}
	if(nArgs>18)
	{
		global_staticalignment = atoi(szArgList[18]);
	}
	//new parameters
	if(nArgs>19)
	{
		wcscpy(szWindowClass, szArgListW[19]); 
	}
	if(nArgs>20)
	{
		wcscpy(szTitle, szArgListW[20]); 
	}
	if(nArgs>21)
	{
		global_begin = szArgList[21]; 
	}
	if(nArgs>22)
	{
		global_end = szArgList[22]; 
	}
	if (nArgs>23)
	{
		global_samplesfolder = szArgList[23];
	}
	if (nArgs>24)
	{
		global_samplesfilter = szArgList[24];
	}	
	//2020august28, spi, begin
	if (nArgs>25)
	{
		global_mididebugmode = szArgList[25]; //"debug" for displaying midi event in the graphic window, "" or anything else for not displaying any events (displaying midi events cause small audio glitches)
	}
	//2020august28, spi,end

	LocalFree(szArgList);
	LocalFree(szArgListW);

	int nShowCmd = false;
	//ShellExecuteA(NULL, "open", "begin.bat", "", NULL, nShowCmd);
	ShellExecuteA(NULL, "open", global_begin.c_str(), "", NULL, nCmdShow);


	//////////////////////////
	//initialize random number
	//////////////////////////
	srand(GetTickCount());


	pFILE = fopen("devices.txt", "w");
	pFILE2 = fopen("samples.txt", "w");

	///////////////////////
	//initialize port audio
	///////////////////////
	global_err = Pa_Initialize();
	if (global_err != paNoError)
	{
		//MessageBox(0,"portaudio initialization failed",0,MB_ICONERROR);
		if (pFILE) fprintf(pFILE, "portaudio initialization failed.\n");
		if(pFILE) fclose(pFILE);
		return 1;
	}

	////////////////////////
	//audio device selection
	////////////////////////
	//SelectAudioInputDevice();
	SelectAudioOutputDevice();
	if(pFILE) fflush(pFILE);

	///////////////////////
	//set tonic sample rate 
	///////////////////////
	// You don't necessarily have to do this - it will default to 44100 if not set.
	Tonic::setSampleRate(SAMPLE_RATE);

	
	for (int i = 0; i < SPITMIPS_MAXNUMBEROFSAMPLERMODULES; i++)
	{
		global_numberofoctavetoshift[SPITMIPS_MAXNUMBEROFSAMPLERMODULES] = 0;
	}

	for (int i = 0; i < SPITMIPS_MAXNUMBEROFSAMPLERMODULES; i++)
	{
		voiceindex[i] = -1;
	}
	///////////////////////////////////////////
	//find out how many sampler modules to load
	///////////////////////////////////////////
	if (global_samplesfolder.rfind(".txt")!=string::npos)
	{
		global_inputmidichannel = -1; //it will not be used
		ifstream ifs(global_samplesfolder);
		string temp;
		while (getline(ifs, temp))
		{
			global_samplesfolderschoice.push_back(temp);
		}
		if (global_samplesfolderschoice.size()>SPITMIPS_MAXNUMBEROFSAMPLERMODULES)
		{
			//randomly pick folders
			while (global_samplesfolders.size() < SPITMIPS_MAXNUMBEROFSAMPLERMODULES)
			{
				global_samplesfolders.push_back(global_samplesfolderschoice[RandomInt(0, global_samplesfolderschoice.size() - 1)]);
			}
		}
		else
		{
			global_samplesfolders = global_samplesfolderschoice;
		}
	}
	else
	{
		global_samplesfolders.push_back(global_samplesfolder);
	}
	global_numberofsamplermodules = global_samplesfolders.size();
	assert(global_numberofsamplermodules>=1 && global_numberofsamplermodules <= SPITMIPS_MAXNUMBEROFSAMPLERMODULES);
	if (pFILE2)
	{
		fprintf(pFILE2, "will load %d sampler module(s)\n", global_numberofsamplermodules);
		fflush(pFILE2);
	}

	//////////////////////////////////////////
	//load in samples for each sampler modules
	//////////////////////////////////////////
	for (global_samplermodulesindex = 0; global_samplermodulesindex < global_numberofsamplermodules; global_samplermodulesindex++)
	{
		if (pFILE2)
		{
			fprintf(pFILE2, "**********************************************************************************************************************\n");
			fprintf(pFILE2, "module id %d on midi channel id %d\n", global_samplermodulesindex, global_samplermodulesindex);
			fprintf(pFILE2, "loading samples in %s\n", global_samplesfolders[global_samplermodulesindex].c_str());
			fprintf(pFILE2, "**********************************************************************************************************************\n");
			fflush(pFILE2);
		}
		loadSynthSamples(global_samplesfolders[global_samplermodulesindex], global_samplesfilter);
		//poly.addVoices(createSynthVoice, 8);
		poly[global_samplermodulesindex].addVoices(createSynthVoice, SPITMIPS_NUMBEROFVOICES);
	}

	StereoDelay delay = StereoDelay(3.0f, 3.0f)
		.delayTimeLeft(0.25 + SineWave().freq(0.2) * 0.01)
		.delayTimeRight(0.30 + SineWave().freq(0.23) * 0.01)
		.feedback(0.4)
		.dryLevel(0.8)
		.wetLevel(0.2);

	for (global_samplermodulesindex = 0; global_samplermodulesindex < global_numberofsamplermodules; global_samplermodulesindex++)
	{
		synth.setOutputGen(synth.getOutputGen() + poly[global_samplermodulesindex]);
	}

	//synth.setOutputGen(poly >> delay);
	synth.setOutputGen(synth.getOutputGen() >> delay);

	//////////////
	//setup stream  
	//////////////
	global_err = Pa_OpenStream(
		&global_stream,
		NULL, //NULL, //&global_inputParameters,
		&global_outputParameters,
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		0, //paClipOff,      // we won't output out of range samples so don't bother clipping them
		renderCallback,
		NULL); //no callback userData
	if (global_err != paNoError)
	{
		char errorbuf[2048];
		sprintf(errorbuf, "Unable to open stream: %s\n", Pa_GetErrorText(global_err));
		//MessageBox(0,errorbuf,0,MB_ICONERROR);
		if (pFILE) fprintf(pFILE, "%s\n", errorbuf);
		if (pFILE) fclose(pFILE);
		return 1;
	}



	//////////////
	//start stream  
	//////////////
	global_err = Pa_StartStream(global_stream);
	if (global_err != paNoError)
	{
		char errorbuf[2048];
		sprintf(errorbuf, "Unable to start stream: %s\n", Pa_GetErrorText(global_err));
		//MessageBox(0,errorbuf,0,MB_ICONERROR);
		if (pFILE) fprintf(pFILE, "%s\n", errorbuf);
		if (pFILE) fclose(pFILE);
		return 1;
	}



	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	//LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//LoadString(hInstance, IDC_SPIWAVWIN32, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	if(global_acceleratoractive)
	{
		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPIWAVWIN32));
	}
	else
	{
		hAccelTable = NULL;
	}
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	//wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPIWAVWIN32));
	wcex.hIcon			= (HICON)LoadImage(NULL, L"background_32x32x16.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);

	if(global_menubardisplay)
	{
		wcex.lpszMenuName = MAKEINTRESOURCE(IDC_SPIWAVWIN32); //original with menu
	}
	else
	{
		wcex.lpszMenuName = NULL; //no menu
	}
	wcex.lpszClassName	= szWindowClass;
	//wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hIconSm		= (HICON)LoadImage(NULL, L"background_16x16x16.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	global_dib = FreeImage_Load(FIF_JPEG, "background.jpg", JPEG_DEFAULT);


	FIBITMAP* local_16x16xrgbdib = FreeImage_Rescale(global_dib, 16, 16, FILTER_BICUBIC);
	FreeImage_Save(FIF_ICO, local_16x16xrgbdib, "background_16x16xrgb-new.ico");
	FreeImage_Unload(local_16x16xrgbdib);

	FIBITMAP* local_32x32xrgbdib = FreeImage_Rescale(global_dib, 32, 32, FILTER_BICUBIC);
	FreeImage_Save(FIF_ICO, local_32x32xrgbdib, "background_32x32xrgb-new.ico");
	FreeImage_Unload(local_32x32xrgbdib);

	FIBITMAP* local_48x48xrgbdib = FreeImage_Rescale(global_dib, 48, 48, FILTER_BICUBIC);
	FreeImage_Save(FIF_ICO, local_48x48xrgbdib, "background_48x48xrgb-new.ico");
	FreeImage_Unload(local_48x48xrgbdib);


	//global_hFont=CreateFontW(32,0,0,0,FW_BOLD,0,0,0,0,0,0,2,0,L"SYSTEM_FIXED_FONT");
	global_hFont=CreateFontW(global_fontheight,0,0,0,FW_NORMAL,0,0,0,0,0,0,2,0,L"SYSTEM_FIXED_FONT");

	if(global_titlebardisplay)
	{
		hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, //original with WS_CAPTION etc.
			global_x, global_y, global_xwidth, global_yheight, NULL, NULL, hInstance, NULL);
	}
	else
	{
		hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUP | WS_VISIBLE, //no WS_CAPTION etc.
			global_x, global_y, global_xwidth, global_yheight, NULL, NULL, hInstance, NULL);
	}
	if (!hWnd)
	{
		return FALSE;
	}
	global_hwnd = hWnd;

	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hWnd, 0, global_alpha, LWA_ALPHA);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	HGDIOBJ hOldBrush;
	HGDIOBJ hOldPen;
	int iOldMixMode;
	COLORREF crOldBkColor;
	COLORREF crOldTextColor;
	int iOldBkMode;
	HFONT hOldFont, hFont;
	TEXTMETRIC myTEXTMETRIC;

	switch (message)
	{
	case WM_CREATE:
		{
			//HWND hStatic = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTER,  
			HWND hStatic = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"", WS_CHILD | WS_VISIBLE | global_staticalignment, 
				0, 100, 100, 100, hWnd, (HMENU)IDC_MAIN_STATIC, GetModuleHandle(NULL), NULL);
			if(hStatic == NULL)
				MessageBox(hWnd, L"Could not create static text.", L"Error", MB_OK | MB_ICONERROR);
			SendMessage(hStatic, WM_SETFONT, (WPARAM)global_hFont, MAKELPARAM(FALSE, 0));



			global_timer=timeSetEvent(1000,25,(LPTIMECALLBACK)&StartGlobalProcess,0,TIME_ONESHOT);
		}
		break;
	case WM_SIZE:
		{
			RECT rcClient;

			GetClientRect(hWnd, &rcClient);
			/*
			HWND hEdit = GetDlgItem(hWnd, IDC_MAIN_EDIT);
			SetWindowPos(hEdit, NULL, 0, 0, rcClient.right/2, rcClient.bottom/2, SWP_NOZORDER);
			*/
			HWND hStatic = GetDlgItem(hWnd, IDC_MAIN_STATIC);
			global_staticwidth = rcClient.right - 0;
			//global_staticheight = rcClient.bottom-(rcClient.bottom/2);
			global_staticheight = rcClient.bottom - 0;

			//spi, begin
			global_imagewidth = rcClient.right - 0;
			global_imageheight = rcClient.bottom - 0; 
			WavSetLib_Initialize(global_hwnd, IDC_MAIN_STATIC, global_staticwidth, global_staticheight, global_fontwidth, global_fontheight, global_staticalignment, global_pfile);
			//spi, end
			//SetWindowPos(hStatic, NULL, 0, rcClient.bottom/2, global_staticwidth, global_staticheight, SWP_NOZORDER);
			SetWindowPos(hStatic, NULL, 0, 0, global_staticwidth, global_staticheight, SWP_NOZORDER);
		}
		break;
	case WM_CTLCOLOREDIT:
		{
			SetBkMode((HDC)wParam, TRANSPARENT);
			SetTextColor((HDC)wParam, RGB(0xFF, 0xFF, 0xFF));
			return (INT_PTR)::GetStockObject(NULL_PEN);
		}
		break;
	case WM_CTLCOLORSTATIC:
		{
			SetBkMode((HDC)wParam, TRANSPARENT);
			//SetTextColor((HDC)wParam, RGB(0xFF, 0xFF, 0xFF));
			SetTextColor((HDC)wParam, RGB(global_fontcolor_r, global_fontcolor_g, global_fontcolor_b));
			return (INT_PTR)::GetStockObject(NULL_PEN);
		}
		break;
	case WM_LBUTTONDOWN:
		{	
			/*
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			*/
			global_samplermodulesindex_selected++;
			if (global_samplermodulesindex_selected == global_numberofsamplermodules) global_samplermodulesindex_selected = 0;
			swprintf(pWCHAR, L"global_samplermodulesindex_selected is %d\n", global_samplermodulesindex_selected); StatusAddText(pWCHAR);
		}
	break;
	case WM_RBUTTONDOWN:
		{
			/*
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			*/
			global_samplermodulesindex_selected--;
			if (global_samplermodulesindex_selected < 0) global_samplermodulesindex_selected = global_numberofsamplermodules-1;
			swprintf(pWCHAR, L"global_samplermodulesindex_selected is %d\n", global_samplermodulesindex_selected); StatusAddText(pWCHAR);
		}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_OEM_MINUS) //For any country/region, the '-' key
		{
			if (global_input_string == "") global_input_string = "-";
			sprintf(pCHAR, "global_input_string is %s\n", global_input_string.c_str()); StatusAddText(pCHAR);
		}
		else if (wParam >= 0x30 && wParam <= 0x39) //key between 0 and 9 inclusively
		{
			if (global_input_string.length() > 3)
			{
				global_input_string = "";
			}			
			else
			{
				char aChar = wParam;
				global_input_string += aChar;
			}
			sprintf(pCHAR, "global_input_string is %s\n", global_input_string.c_str()); StatusAddText(pCHAR);
		}
		else if ( (wParam == 0x4D) || (wParam == VK_RETURN) ) // M key or RETURN key
		{
			int inputmidichannel = atoi(global_input_string.c_str());
			if (inputmidichannel >= -3 && inputmidichannel <= 15)
			{
				global_inputmidichannel = inputmidichannel;
				global_input_string = "";
			}
			swprintf(pWCHAR, L"global_inputmidichannel is %d\n", global_inputmidichannel); StatusAddText(pWCHAR);
		}
		else if ((wParam == 0x51) || (wParam == VK_UP)) // Q key or UP ARROW key
		{
			global_numberofoctavetoshift[global_samplermodulesindex_selected]++;
			if (global_numberofoctavetoshift[global_samplermodulesindex_selected] > 11) global_numberofoctavetoshift[global_samplermodulesindex_selected] = 11;
			swprintf(pWCHAR, L"transposing %d octave all midi events mapping to sampler module %d\n", global_numberofoctavetoshift[global_samplermodulesindex_selected] , global_samplermodulesindex_selected); StatusAddText(pWCHAR);
		}
		else if ((wParam == 0x5A) || (wParam == VK_DOWN)) // Z key or DOWN ARROW key
		{
			global_numberofoctavetoshift[global_samplermodulesindex_selected]--;
			if (global_numberofoctavetoshift[global_samplermodulesindex_selected] < -11) global_numberofoctavetoshift[global_samplermodulesindex_selected] = -11;
			swprintf(pWCHAR, L"transposing %d octave all midi events mapping to sampler module %d\n", global_numberofoctavetoshift[global_samplermodulesindex_selected], global_samplermodulesindex_selected); StatusAddText(pWCHAR);
		}
		else if ((wParam == VK_BACK) || (wParam == VK_DELETE)) //backspace key
		{
			global_input_string = "";
		}
		else if (wParam == 0x43) //C key
		{
			if (global_audiomidi_devices == "connected")
			{
				////////////////////
				//terminate portmidi
				////////////////////
				global_active = false;
				Pm_Close(global_pPmStreamMIDIIN);
				Pt_Stop();
				Pm_Terminate();
				//spi, begin
				/////////////////////
				//terminate portaudio
				/////////////////////
				global_err = Pa_StopStream(global_stream);
				if (global_err != paNoError)
				{
					char errorbuf[2048];
					sprintf(errorbuf, "Error stoping stream: %s\n", Pa_GetErrorText(global_err));
					MessageBoxA(0, errorbuf, 0, MB_ICONERROR);
					return 1;
				}
				global_err = Pa_CloseStream(global_stream);
				if (global_err != paNoError)
				{
					char errorbuf[2048];
					sprintf(errorbuf, "Error closing stream: %s\n", Pa_GetErrorText(global_err));
					MessageBoxA(0, errorbuf, 0, MB_ICONERROR);
					return 1;
				}
				Pa_Terminate();
				swprintf(pWCHAR, L"spitonicmidiinstrumentpolysamplerswin32 disconnected.\n"); StatusAddText(pWCHAR);
				global_audiomidi_devices = "disconnected";
			}
			else if (global_audiomidi_devices == "disconnected")
			{
				///////////////////////
				//initialize port audio
				///////////////////////
				global_err = Pa_Initialize();
				if (global_err != paNoError)
				{
					//MessageBox(0,"portaudio initialization failed",0,MB_ICONERROR);
					if (pFILE) fprintf(pFILE, "portaudio initialization failed.\n");
					if (pFILE) fclose(pFILE);
					return 1;
				}

				////////////////////////
				//audio device selection
				////////////////////////
				//SelectAudioInputDevice();
				SelectAudioOutputDevice();

				////////////////////
				//setup audio stream  
				////////////////////
				global_err = Pa_OpenStream(
					&global_stream,
					NULL, //NULL, //&global_inputParameters,
					&global_outputParameters,
					SAMPLE_RATE,
					FRAMES_PER_BUFFER,
					0, //paClipOff,      // we won't output out of range samples so don't bother clipping them
					renderCallback,
					NULL); //no callback userData
				if (global_err != paNoError)
				{
					char errorbuf[2048];
					sprintf(errorbuf, "Unable to open stream: %s\n", Pa_GetErrorText(global_err));
					//MessageBox(0,errorbuf,0,MB_ICONERROR);
					if (pFILE) fprintf(pFILE, "%s\n", errorbuf);
					if (pFILE) fclose(pFILE);
					return 1;
				}

				////////////////////
				//start audio stream  
				////////////////////
				global_err = Pa_StartStream(global_stream);
				if (global_err != paNoError)
				{
					char errorbuf[2048];
					sprintf(errorbuf, "Unable to start stream: %s\n", Pa_GetErrorText(global_err));
					//MessageBox(0,errorbuf,0,MB_ICONERROR);
					if (pFILE) fprintf(pFILE, "%s\n", errorbuf);
					if (pFILE) fclose(pFILE);
					return 1;
				}

				/////////////////////
				//initialize portmidi
				/////////////////////
				PmError err;
				err = Pm_Initialize();
				if (err)
				{
					sprintf(pCHAR, Pm_GetErrorText(err)); StatusAddTextA(pCHAR);
					if (pFILE) fprintf(pFILE, "error,  Pm_Initialize() failed returning error %s\n", pCHAR);
					if (pFILE) fflush(pFILE);
					//Pt_Stop();
					//Terminate();
					//mmexit(1);
					return 1;
				}
				

				/////////////////////////////
				//input midi device selection
				/////////////////////////////
				const PmDeviceInfo* deviceInfo;
				int numDevices = Pm_CountDevices();
				for (int i = 0; i<numDevices; i++)
				{
					deviceInfo = Pm_GetDeviceInfo(i);
					if (deviceInfo->input)
					{
						string devicenamestring = deviceInfo->name;
						global_inputmididevicemap.insert(pair<string, int>(devicenamestring, i));
					}
				}
				std::map<string, int>::iterator it;
				it = global_inputmididevicemap.find(global_inputmididevicename);
				if (it != global_inputmididevicemap.end())
				{
					global_inputmidideviceid = (*it).second;
					sprintf(pCHAR, "%s maps to %d\n", global_inputmididevicename.c_str(), global_inputmidideviceid); StatusAddTextA(pCHAR); //spi note: crashes on first VS2013 compile
					if (pFILE) fprintf(pFILE, "found input midi device %s\n", pCHAR);
					if (pFILE) fflush(pFILE);
					deviceInfo = Pm_GetDeviceInfo(global_inputmidideviceid);
				}
				else
				{
					assert(false);
					for (it = global_inputmididevicemap.begin(); it != global_inputmididevicemap.end(); it++)
					{
						sprintf(pCHAR, "%s maps to %d\n", (*it).first.c_str(), (*it).second); StatusAddTextA(pCHAR);
					}
					swprintf(pWCHAR, L"input midi device not found\n"); StatusAddText(pWCHAR);
					if (pFILE) fwprintf(pFILE, L"%s\n", pWCHAR);
					if (pFILE) fflush(pFILE);
					return 1;
				}

				// use porttime callback to empty midi queue and print 
				PtError pterr = Pt_Start(1, receive_poll, 0); //Pt_Start(1, receive_poll, global_pInstrument); 
				if (pterr)
				{
					if (pFILE) fprintf(pFILE, "error,  Pt_Start() failed returning error %d\n", pterr);
					if (pFILE) fflush(pFILE);
					//Pt_Stop();
					//Terminate();
					//mmexit(1);
					return 1;
				}

				// list device information 
				swprintf(pWCHAR, L"MIDI input devices:\n"); StatusAddText(pWCHAR);
				if (pFILE) fwprintf(pFILE, L"%s\n", pWCHAR);
				for (int i = 0; i < Pm_CountDevices(); i++)
				{
					const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
					if (info->input)
					{
						sprintf(pCHAR, "%d: %s, %s\n", i, info->interf, info->name); StatusAddTextA(pCHAR);
						if (pFILE) fprintf(pFILE, "%s\n", pCHAR);
					}
				}
				//inputmididevice = get_number("Type input device number: ");
				swprintf(pWCHAR, L"device %d selected\n", global_inputmidideviceid); StatusAddText(pWCHAR);
				if (pFILE) fwprintf(pFILE, L"%s\n", pWCHAR);
				if (pFILE) fflush(pFILE);
				err = Pm_OpenInput(&global_pPmStreamMIDIIN, global_inputmidideviceid, NULL, 512, NULL, NULL);
				if (err)
				{
					sprintf(pCHAR, Pm_GetErrorText(err)); StatusAddTextA(pCHAR);
					Pt_Stop();
					//Terminate();
					//mmexit(1);
					return 1;
				}
				Pm_SetFilter(global_pPmStreamMIDIIN, mySpiMidiUtility.filter);
				global_inited = true; // now can document changes, set filter 
				swprintf(pWCHAR, L"spitonicmidiinstrumentpolysamplerswin32 ready.\n"); StatusAddText(pWCHAR);
				global_active = true;

				global_audiomidi_devices = "connected";
			}
			
		}
		else if (wParam == 0x44) //D key
		{
			if (global_mididebugmode == "debug") global_mididebugmode = "";
			  else global_mididebugmode = "debug";
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//spi, begin
		
		SetStretchBltMode(hdc, COLORONCOLOR);
		
		StretchDIBits(hdc, 0, 0, global_imagewidth, global_imageheight,
						0, 0, FreeImage_GetWidth(global_dib), FreeImage_GetHeight(global_dib),
						FreeImage_GetBits(global_dib), FreeImage_GetInfo(global_dib), DIB_RGB_COLORS, SRCCOPY);
		
		//spi, end
		hOldBrush = SelectObject(hdc, (HBRUSH)GetStockObject(GRAY_BRUSH));
		hOldPen = SelectObject(hdc, (HPEN)GetStockObject(WHITE_PEN));
		//iOldMixMode = SetROP2(hdc, R2_MASKPEN);
		iOldMixMode = SetROP2(hdc, R2_MERGEPEN);
		//Rectangle(hdc, 100, 100, 200, 200);

		crOldBkColor = SetBkColor(hdc, RGB(0xFF, 0x00, 0x00));
		crOldTextColor = SetTextColor(hdc, RGB(0xFF, 0xFF, 0xFF));
		iOldBkMode = SetBkMode(hdc, TRANSPARENT);
		//hFont=CreateFontW(70,0,0,0,FW_BOLD,0,0,0,0,0,0,2,0,L"SYSTEM_FIXED_FONT");
		//hOldFont=(HFONT)SelectObject(hdc,global_hFont);
		hOldFont=(HFONT)SelectObject(hdc,global_hFont);
		GetTextMetrics(hdc, &myTEXTMETRIC);
		global_fontwidth = myTEXTMETRIC.tmAveCharWidth;
		//TextOutW(hdc, 100, 100, L"test string", 11);

		SelectObject(hdc, hOldBrush);
		SelectObject(hdc, hOldPen);
		SetROP2(hdc, iOldMixMode);
		SetBkColor(hdc, crOldBkColor);
		SetTextColor(hdc, crOldTextColor);
		SetBkMode(hdc, iOldBkMode);
		SelectObject(hdc,hOldFont);
		//DeleteObject(hFont);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		{
			////////////////////
			//terminate portmidi
			////////////////////
			global_active = false;
			Pm_Close(global_pPmStreamMIDIIN);
			Pt_Stop();
			Pm_Terminate();
			//spi, begin
			/////////////////////
			//terminate portaudio
			/////////////////////
			global_err = Pa_StopStream(global_stream);
			if (global_err != paNoError)
			{
				char errorbuf[2048];
				sprintf(errorbuf, "Error stoping stream: %s\n", Pa_GetErrorText(global_err));
				MessageBoxA(0, errorbuf, 0, MB_ICONERROR);
				return 1;
			}
			global_err = Pa_CloseStream(global_stream);
			if (global_err != paNoError)
			{
				char errorbuf[2048];
				sprintf(errorbuf, "Error closing stream: %s\n", Pa_GetErrorText(global_err));
				MessageBoxA(0, errorbuf, 0, MB_ICONERROR);
				return 1;
			}
			Pa_Terminate();
			//spi, end
			//delete all memory allocations
			for (global_samplermodulesindex = 0; global_samplermodulesindex < global_numberofsamplermodules; global_samplermodulesindex++)
			{
				unloadSynthSamples();
			}
			//if(global_pInstrument) delete global_pInstrument;
			//close file
			if(global_pfile) fclose(global_pfile);
			if (pFILE) fclose(pFILE); //added by spi
			if (pFILE2) fclose(pFILE2); //added by spi

			//terminate wavset library
			WavSetLib_Terminate();
			//terminate win32 app.
			if (global_timer) timeKillEvent(global_timer);
			FreeImage_Unload(global_dib);
			DeleteObject(global_hFont);

			int nShowCmd = false;
			//ShellExecuteA(NULL, "open", "end.bat", "", NULL, nShowCmd);
			ShellExecuteA(NULL, "open", global_end.c_str(), "", NULL, 0);
			PostQuitMessage(0);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
