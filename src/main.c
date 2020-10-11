/*
    Copyright 2015-2020 Robert Tari <robert@tari.in>

    This file is part of Odio SACD library.

    Odio SACD library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Odio SACD library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Odio SACD library. If not, see <http://www.gnu.org/licenses/gpl-3.0.txt>.
*/

#include <libodiosacd/libodiosacd.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#define APPVERSION "20.5.12"

bool m_bContinue = true;

static bool odiosacd_OnProgress(float fProgress, char *sFilePath, int nTrack, void *pUserData)
{
    if (m_bContinue)
    {
        printf("\r%.2f%%", fProgress);
    }
    else
    {
        printf("\rClosing...");
    }

    fflush(stdout);

    return m_bContinue;
}

static void onClose(int nSignal)
{
    if (nSignal == SIGTERM || nSignal == SIGINT || nSignal == SIGQUIT)
    {
        m_bContinue = false;
    }
}

int main(int argc, char *argv[])
{
    signal(SIGTERM, onClose);
    signal(SIGINT, onClose);
    signal(SIGQUIT, onClose);

    const char strHelpText[] =
    "\n"
    "Usage: odio-sacd -i /path/to/file [options]\n\n"
    "  -i, --infile         : Specify the input file (*.iso, *.dsf, *.dff)\n"
    "  -o, --outdir         : The folder to write the WAVE files to. If you omit\n"
    "                         this, the files will be placed in the input file's\n"
    "                         directory\n"
    "  -r, --rate           : The output samplerate.\n"
    "                         Valid rates are: 88200 and 176400.\n"
    "                         If you omit this, 88.2kHz will be used.\n"
    "  -s, --stereo         : Only extract the 2-channel area if it exists.\n"
    "                         If you omit this, the multichannel area will have priority.\n"
    "  -d, --details        : Show detailed information about the disc\n\n";

    const struct option lOptions[] =
    {
        {"infile", required_argument, NULL, 'i' },
        {"outdir", required_argument, NULL, 'o' },
        {"rate", required_argument, NULL, 'r' },
        {"stereo", no_argument, NULL, 's'},
        {"details", no_argument, NULL, 'd'},
        { NULL, 0, NULL, 0 }
    };

    int nOpt;
    int nSampleRate = 88200;
    char *sInPath = NULL;
    char *sOutPath = NULL;
    bool bPrintDetails = false;
    bool bPrintHelp = false;
    Area nArea = AREA_AUTO;

    while ((nOpt = getopt_long(argc, argv, "i:o:r:sd", lOptions, NULL)) >= 0)
    {
        switch (nOpt)
        {
            case 'i':
            {
                sInPath = optarg;

                break;
            }
            case 'o':
            {
                sOutPath = optarg;

                break;
            }
            case 'r':
            {
                if (!strcmp(optarg, "88200") || !strcmp(optarg, "176400"))
                {
                    nSampleRate = atoi(optarg);
                }
                else
                {
                    printf("\nPANIC: Invalid samplerate\n%s", strHelpText);

                    return 1;
                }

                break;
            }
            case 's':
            {
                nArea = AREA_TWOCH;

                break;
            }
            case 'd':
            {
                bPrintDetails = true;

                break;
            }
            default:
            {
                bPrintHelp = true;

                break;
            }
        }
    }

    if (bPrintHelp || argc == 1 || sInPath == NULL)
    {
        printf("\nPANIC: Invalid command-line syntax\n%s\n", strHelpText);

        return 1;
    }

    printf("\n\nOdio SACD v%s\n--------------------\nCommand-line decoder\n\n", APPVERSION);
    bool bError = odiolibsacd_Open(sInPath, nArea);

    if (bError)
    {
        printf("\n");

        return 1;
    }

    if (bPrintDetails)
    {
        DiscDetails *pDiscDetails = odiolibsacd_GetDiscDetails();

        if (pDiscDetails)
        {
            if (pDiscDetails->sAlbumTitle && strlen(pDiscDetails->sAlbumTitle))
            {
                printf("ALBUM TITLE: %s\n", pDiscDetails->sAlbumTitle);
            }

            if (pDiscDetails->sAlbumTitle && strlen(pDiscDetails->sAlbumArtist))
            {
                printf("ALBUM ARTIST: %s\n", pDiscDetails->sAlbumArtist);
            }

            if (pDiscDetails->sAlbumPublisher && strlen(pDiscDetails->sAlbumPublisher))
            {
                printf("ALBUM PUBLISHER: %s\n", pDiscDetails->sAlbumPublisher);
            }

            if (pDiscDetails->sAlbumCopyright && strlen(pDiscDetails->sAlbumCopyright))
            {
                printf("ALBUM COPYRIGHT: %s\n", pDiscDetails->sAlbumCopyright);
            }

            for (int nTrack = 0; nTrack < pDiscDetails->nTwoChTracks; nTrack++)
            {
                printf("\n");
                printf("TWO-CHANNEL AREA TRACK %.2i PERFORMER: %s\n", nTrack + 1, pDiscDetails->lTwoChTrackDetails[nTrack].sTrackPerformer);
                printf("TWO-CHANNEL AREA TRACK %.2i TITLE: %s\n", nTrack + 1, pDiscDetails->lTwoChTrackDetails[nTrack].sTrackTitle);
                printf("TWO-CHANNEL AREA TRACK %.2i CHANNELS: %i\n", nTrack + 1, pDiscDetails->lTwoChTrackDetails[nTrack].nChannels);
            }

            for (int nTrack = 0; nTrack < pDiscDetails->nMulChTracks; nTrack++)
            {
                printf("\n");
                printf("MULTI-CHANNEL AREA TRACK %.2i PERFORMER: %s\n", nTrack + 1, pDiscDetails->lMulChTrackDetails[nTrack].sTrackPerformer);
                printf("MULTI-CHANNEL AREA TRACK %.2i TITLE: %s\n", nTrack + 1, pDiscDetails->lMulChTrackDetails[nTrack].sTrackTitle);
                printf("MULTI-CHANNEL AREA TRACK %.2i CHANNELS: %i\n", nTrack + 1, pDiscDetails->lMulChTrackDetails[nTrack].nChannels);
            }

            printf("\n");
        }

        return 0;
    }

    time_t nNow = time(0);
    bError = odiolibsacd_Convert(sOutPath, nSampleRate, odiosacd_OnProgress, NULL);

    if (bError)
    {
        printf("\rConversion failed\n\n");
        fflush(stdout);
        odiolibsacd_Close();

        return 1;
    }

    if (m_bContinue)
    {
        printf("\rFinished in %i seconds.\n\n", (int)(time(0) - nNow));
    }
    else
    {
        printf("\rCancelled.\n\n");
    }

    fflush(stdout);
    odiolibsacd_Close();

    return 0;
}
