/*------------------------------------------------------------------------------
 * rnx2rtkp.c : read rinex obs/nav files and compute receiver positions
 *
 *          Copyright (C) 2007-2016 by T.TAKASU, All rights reserved.
 *
 * version : $Revision: 1.1 $ $Date: 2008/07/17 21:55:16 $
 * history : 2007/01/16  1.0 new
 *           2007/03/15  1.1 add library mode
 *           2007/05/08  1.2 separate from postpos.c
 *           2009/01/20  1.3 support rtklib 2.2.0 api
 *           2009/12/12  1.4 support glonass
 *                           add option -h, -a, -l, -x
 *           2010/01/28  1.5 add option -k
 *           2010/08/12  1.6 add option -y implementation (2.4.0_p1)
 *           2014/01/27  1.7 fix bug on default output time format
 *           2015/05/15  1.8 -r or -l options for fixed or ppp-fixed mode
 *           2015/06/12  1.9 output patch level in header
 *           2016/09/07  1.10 add option -sys
 *-----------------------------------------------------------------------------*/
#include <stdarg.h>
#include <string>
#include "rtklib.h"

#define MAXFILE 16 /* max number of input files */

/* show message --------------------------------------------------------------*/
extern int showmsg(char *format, ...) {
  va_list arg;
  va_start(arg, format);
  vfprintf(stderr, format, arg);
  va_end(arg);
  fprintf(stderr, "\r");
  return 0;
}
extern void settspan(gtime_t sGTimeStart, gtime_t sGTimeEnd) {}
extern void settime(gtime_t time) {}

/* rnx2rtkp main -------------------------------------------------------------*/
int main(int argc, char **argv) {
  prcopt_t sProcessOption = prcopt_default;
  solopt_t sSolutionOption = solopt_default;
  filopt_t sFileOption = {""};
  gtime_t sGTimeStart = {0}, sGTimeEnd = {0};
  double PrcsInterval = 0.0;
  int i, ret;  //, j
  char *archInFiles[MAXFILE] = {NULL},
       *archOutFiles = "";  // "../products/CHAN.txt", *p

  sProcessOption.mode = PMODE_KINEMA;
  sProcessOption.navsys = 0;
  sProcessOption.refpos = 1;
  sProcessOption.glomodear = 1;
  sSolutionOption.timef = 0;

  /* load options from configuration file */
  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-k") && i + 1 < argc) {
      resetsysopts();
      if (!loadopts(argv[++i], sysopts)) return -1;
      getsysopts(&sProcessOption, &sSolutionOption, &sFileOption);
    }
  }
  double arTimeStart[] = {
      sProcessOption.sTimeStart.Year,   sProcessOption.sTimeStart.Month,
      sProcessOption.sTimeStart.Day,    sProcessOption.sTimeStart.Hour,
      sProcessOption.sTimeStart.Minute, sProcessOption.sTimeStart.Second};
  double arTimeEnd[] = {sProcessOption.sTimeEnd.Year,
                        sProcessOption.sTimeEnd.Month,
                        sProcessOption.sTimeEnd.Day,
                        sProcessOption.sTimeEnd.Hour,
                        sProcessOption.sTimeEnd.Minute,
                        sProcessOption.sTimeEnd.Second};  //, pos[3]
  sGTimeStart = epoch2time(arTimeStart);
  sGTimeEnd = epoch2time(arTimeEnd);

  if (!sProcessOption.navsys) {
    sProcessOption.navsys = SYS_GPS | SYS_GLO;
  }
  for (int i = 0; i < MAXFILE; i++) {
    if (NULL == archInFiles[i]) {
      archInFiles[i] = new char[MAXSTRPATH];
    }
  }
  int NumInFiles = 0;
  strcpy(archInFiles[NumInFiles++], sFileOption.obsfile);
  strcpy(archInFiles[NumInFiles++], sFileOption.navfile);
  // base
  if (sFileOption.basefile[0] != '\0')
    strcpy(archInFiles[NumInFiles++], sFileOption.basefile);
  // precise orbit
  if (sFileOption.orbitfile[0] != '\0')
    strcpy(archInFiles[NumInFiles++], sFileOption.orbitfile);
  // precise clock
  if (sFileOption.clkfile[0] != '\0')
    strcpy(archInFiles[NumInFiles++], sFileOption.clkfile);
  if (NumInFiles <= 0) {
    showmsg("error : no input file");
    return -2;
  }

  // Output Name
  if(sFileOption.file_id[0] == '\0') {
    std::string OfileName = sFileOption.obsfile;
    std::string str1 = OfileName.substr(0, OfileName.find('.'));
    std::string str2 = str1.substr(str1.find_last_of("/") + 1);
    for (size_t k = 0; k < str2.size(); k++)
      sFileOption.file_id[k] = str2[k];
  }

  ret = postpos(sGTimeStart, sGTimeEnd, PrcsInterval, 0.0, &sProcessOption,
                &sSolutionOption, &sFileOption, archInFiles, NumInFiles,
                archOutFiles, "", "");

  if (!ret) fprintf(stderr, "%40s\r", "");
  return ret;
}
