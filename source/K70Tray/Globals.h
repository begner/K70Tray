
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "Constants.h"


using namespace std;


extern			void		DebugMsg(string msg, ...);
extern			void		addThemeToDropdown(string themename);
extern			void		addLayoutToDropdown(string layoutname);
extern			void		SetCurrentLayout(string layoutname);
extern			void		SetCurrentTheme(string layoutname);