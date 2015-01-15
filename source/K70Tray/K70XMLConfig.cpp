#include "stdafx.h"
#include "K70XMLConfig.h"

#include "tinyxml.cpp"
#include "tinystr.cpp"
#include "tinyxmlerror.cpp"
#include "tinyxmlparser.cpp"
#include "MainCorsairRGBK.h"
#include "BoardAnimation.h"



using namespace std;

extern unsigned char		g_ledAdress[K70_KEY_MAX];
extern unsigned char		g_XYk[K70_ROWS][K70_COLS];
extern unsigned char		g_keyCodes[K70_KEY_MAX];
extern float				g_keySizes[K70_KEY_MAX];
extern string				g_keyNames[K70_KEY_MAX];
extern int					g_iInterval;
extern KeyboardDevice		* keyBoardDevice;
extern Theme				* currentTheme;
extern HWND					ghDlgMain;

K70XMLConfig::K70XMLConfig(MainCorsairRGBK * mainRef)
{
	DebugMsg("XMLConfig Initialized\n");
}

string K70XMLConfig::getLayoutName() {
	return layoutName;
}

string K70XMLConfig::getThemeName() {
	return themeName;
}

int K70XMLConfig::getLastWindowPosition(string pos) {
	if (pos == string("x")) {
		return windowPositionLeft;
	}
	else {
		return windowPositionTop;
	}
}





bool K70XMLConfig::readConfig()
{

	DebugMsg("--------- CONFIG ------------");
	TiXmlDocument doc("config.xml");

	if (doc.LoadFile())
	{
		TiXmlHandle  handle(&doc);

		const char * ln = handle.FirstChild("Config").FirstChild("Layout").ToElement()->GetText();
		DebugMsg("layoutName: %s", ln);
		layoutName = ln;

		const char * tn = handle.FirstChild("Config").FirstChild("Theme").ToElement()->GetText();
		DebugMsg("themeName: %s", tn);
		themeName = tn;


		// first find position
		if (handle.FirstChild("Config").FirstChild("WindowPositionTop").ToElement()) {
			windowPositionTop = atoi(handle.FirstChild("Config").FirstChild("WindowPositionTop").ToElement()->GetText());
		}
		if (handle.FirstChild("Config").FirstChild("WindowPositionLeft").ToElement()) {
			windowPositionLeft = atoi(handle.FirstChild("Config").FirstChild("WindowPositionLeft").ToElement()->GetText());
		}

		// ... then open instant - if we need to :)
		const int openWindowOnStartup = atoi(handle.FirstChild("Config").FirstChild("openMainDlg").ToElement()->GetText());
		DebugMsg("openMainDlg: %i", openWindowOnStartup);
		if (openWindowOnStartup > 0) {
			DebugMsg(" - ShowWindow!");
			ShowWindow(ghDlgMain, SW_SHOW);
		}
		


		// parseLayout(layoutName);

		unsigned int updateInterval = atoi(handle.FirstChild("Config").FirstChild("UpdateInterval").ToElement()->GetText());
		
		g_iInterval = updateInterval;
		DebugMsg("Set Interval to %i", g_iInterval);
	
	}
	else {
		DebugMsg("No config.xml found!");
	}
	return true;
}

bool K70XMLConfig::readTheme()
{

	DebugMsg("--------- THEME ------------");
	TiXmlDocument doc("config.xml");

	if (doc.LoadFile())
	{
		TiXmlHandle  handle(&doc);
		const char * themeName = handle.FirstChild("Config").FirstChild("Theme").ToElement()->GetText();
		DebugMsg("ThemeName: %s\n", themeName);
		parseTheme(themeName);

	}
	else {
		DebugMsg("No config.xml found!");
	}
	return true;
}

void K70XMLConfig::initLayout() {
	
	for (int i = 0; i < K70_KEY_MAX; i++) {
		g_keyCodes[i] = 0;
		g_keySizes[i] = 0.25;
		g_ledAdress[i] = 0;
		g_keyNames[i] = string("");
	}

}

bool K70XMLConfig::parseLayout(const char * localLayoutName) {

	initLayout();



	int counter = 0;
	int spaceCounter = 0;

	DebugMsg("--------- LAYOUT ------------\n");

	string layoutFileName;
	layoutFileName = string("Layout.") + string(localLayoutName) + ".xml";
	DebugMsg("layoutFileName: %s", layoutFileName.c_str());




	TiXmlDocument doc(layoutFileName.c_str());
	if (doc.LoadFile())
	{
		TiXmlHandle  handle(&doc);
		TiXmlElement* root = handle.FirstChildElement("Layout").ToElement();

		// get device
		if (!root->Attribute("device")) {
			DebugMsg("WARNING: Attribute 'device' missing in Layout Tag!");
			return FALSE;
		}

		int neededKeysInRow = 0;
		string deviceType = string(root->Attribute("device"));
		if (deviceType == string("K70")) {
			neededKeysInRow = REQUIRED_KEYROWSIZE_K70;
		}
		else if (deviceType == string("K95")) {
			neededKeysInRow = REQUIRED_KEYROWSIZE_K95;
		}
		else {
			DebugMsg("WARNING: Unknown 'device' = '%s'", deviceType.c_str());
			return FALSE;
		}




		int currentRow = 0;
		// iterate rows...
		for (TiXmlElement* row = root->FirstChildElement("Row"); row != NULL; row = row->NextSiblingElement("Row"))
		{
			currentRow++;
			float keysInRow = 0;
			for (TiXmlElement* key = row->FirstChildElement(); key != NULL; key = key->NextSiblingElement())
			{
				string tagName = string(key->Value());
				string keyName = "";
				
				unsigned char keyCode = 0;
				float keySize = 0;
				unsigned char led = 0;

				if (key->Attribute("name")) {
					keyName = string(key->Attribute("name"));
				}
				if (key->Attribute("code")) {
					char * prefix = "0x";
					long int keyCodeLong = 0;
					keyCodeLong = strtol(key->Attribute("code"), &prefix, 16);
					keyCode = (unsigned char)keyCodeLong;
				}
				if (key->Attribute("keysize")) {
					keySize = strtof(key->Attribute("keysize"), NULL);
				}
				if (key->Attribute("led")) {
					long int ledInt = 0;
					ledInt = atoi(key->Attribute("led"));
					led = (unsigned char)ledInt;
				}

				keysInRow += keySize;
				
				
				if (string(tagName) == string("Space")) {
					// DebugMsg("SPACE '%s' %f\n", tagName.c_str(), keySize * -1);
					g_keySizes[counter + spaceCounter] = keySize * -1;
					spaceCounter++;
				}
				if (string(tagName) == string("Key")) {
					// DebugMsg("KEY '%s' %f %x %x\n", keyName.c_str(), keySize, keyCode, led);
					g_keyNames[counter] = keyName;
					g_keyCodes[counter] = keyCode; 
					g_keySizes[counter + spaceCounter] = keySize;
					g_ledAdress[counter] = led; 
					counter++;
				}
				
			}

			if (keysInRow != neededKeysInRow) {
				DebugMsg("WARNING: Not enough keysize in row %i. Is %f - but needs to be %i", currentRow, keysInRow, neededKeysInRow);
				return FALSE;
			}
			g_keyCodes[counter] = 255;
			g_keySizes[counter + spaceCounter] = 0;
			g_ledAdress[counter] = 255;
			counter++;
		}
	}
	else {
		DebugMsg("Layoutfile %s not found!", layoutFileName.c_str());
		return FALSE;
	}
	layoutName = localLayoutName;
	return TRUE;
}

void K70XMLConfig::parseTheme(const char * localThemeName) {
	
	
	string themeFileName;
	themeFileName = string("Theme.") + string(localThemeName) + ".xml";
	DebugMsg("themeFileName: %s", themeFileName.c_str());

	map <string, vector<K70RGB>> sharedAnimationMap;
	map <string, vector<string>> sharedKeyGroupMap;
	map <string, BoardAnimation> sharedBoardAnimationMap;

	TiXmlDocument doc(themeFileName.c_str());
	if (doc.LoadFile())
	{
		TiXmlHandle  handle(&doc);
		TiXmlElement* root = handle.FirstChildElement("Theme").ToElement();
				


		// shared animations
		for (TiXmlElement* sharedAnimationsNode = root->FirstChildElement("sharedAnimations"); sharedAnimationsNode != NULL; sharedAnimationsNode = sharedAnimationsNode->NextSiblingElement("sharedAnimations")) {
			for (TiXmlElement* animationNode = sharedAnimationsNode->FirstChildElement("animation"); animationNode != NULL; animationNode = animationNode->NextSiblingElement("animation")) {
				
				string sharedAnimationName = string(animationNode->Attribute("name"));
				
				unsigned int completeDuration = 0;
				vector<K70RGB> animation;
				for (TiXmlElement* colorNode = animationNode->FirstChildElement("color"); colorNode != NULL; colorNode = colorNode->NextSiblingElement("color")) {
					K70RGB color;
					color.setR(atoi(colorNode->Attribute("r")));
					color.setG(atoi(colorNode->Attribute("g")));
					color.setB(atoi(colorNode->Attribute("b")));
					unsigned int duration = 0;
					if (colorNode->Attribute("duration")) {
						duration = atoi(colorNode->Attribute("duration"));
					}
					if (duration < 1) {
						duration = 1;
					}

					for (unsigned int i = 0; i < duration; i++) {
						animation.push_back(color);
					}
					completeDuration += duration;
				}
				DebugMsg("Loaded SharedAnimation: '%s', duration: %i", sharedAnimationName.c_str(), completeDuration);
				sharedAnimationMap.insert(make_pair(sharedAnimationName, animation));
			}
		}

		// keyGroups
		for (TiXmlElement* sharedKeyGroupNode = root->FirstChildElement("KeyGroups"); sharedKeyGroupNode != NULL; sharedKeyGroupNode = sharedKeyGroupNode->NextSiblingElement("KeyGroups")) {
			for (TiXmlElement* KeyGroupNode = sharedKeyGroupNode->FirstChildElement("group"); KeyGroupNode != NULL; KeyGroupNode = KeyGroupNode->NextSiblingElement("group")) {
				string keyGroupName = "";
				keyGroupName = string(KeyGroupNode->Attribute("name"));
				DebugMsg("Loaded SharedKeyGroup: '%s': ", keyGroupName.c_str());
				vector<string> keyGroup;
				for (TiXmlElement* keyNode = KeyGroupNode->FirstChildElement("key"); keyNode != NULL; keyNode = keyNode->NextSiblingElement("key")) {
					string keyName = string(keyNode->Attribute("name"));
					
					if (currentTheme->getKeyboard()->KeynameExist(keyName)) {
						keyGroup.push_back(keyName);
					}
					else {
						DebugMsg("KEYNAME '%s' DOES NOT EXIST", keyName.c_str());
					}
					/*
					if (keyNode == KeyGroupNode->LastChild("key")) {
						DebugMsg(" - '%s'", keyName.c_str());
					}
					else {
						DebugMsg(" - '%s'", keyName.c_str());
					}
					*/
					
				}
				sharedKeyGroupMap.insert(make_pair(keyGroupName, keyGroup));
			}
		}


		// sharedBoardAnimations
		for (TiXmlElement* sharedBANode = root->FirstChildElement("sharedBoardAnimations"); sharedBANode != NULL; sharedBANode = sharedBANode->NextSiblingElement("sharedBoardAnimations")) {
			
			for (TiXmlElement* baNode = sharedBANode->FirstChildElement("boardAnimation"); baNode != NULL; baNode = baNode->NextSiblingElement("boardAnimation")) {
				if (!baNode->Attribute("name")) {
					DebugMsg(" - WARNING: Attribute 'name' is missing for boardAnimation!");
					continue;
				}
				string baName = string(baNode->Attribute("name"));
				BoardAnimation bAnim;
				bAnim.setName(baName);

				for (TiXmlElement* baANode = baNode->FirstChildElement("animation"); baANode != NULL; baANode = baANode->NextSiblingElement("animation")) {
					if (!baANode->Attribute("x")) {
						DebugMsg(" - WARNING: Attribute 'x' is missing for animation!");
						continue;
					}
					if (!baANode->Attribute("y")) {
						DebugMsg(" - WARNING: Attribute 'y' is missing for animation!");
						continue;
					}
					
					int posX = atoi(baANode->Attribute("x"));
					int posY = atoi(baANode->Attribute("y"))*-1;
					vector<K70RGB>  myColors;

					int start = 0;
					if (baANode->Attribute("start")) {
						start = atoi(baANode->Attribute("start"));
					}


					for (int i = 0; i < start; i++) {
						K70RGB color;
						color.setR(0);
						color.setG(0);
						color.setB(0);
						myColors.push_back(color);
					}

					if (baANode->Attribute("name")) {
						string animationName = string(baANode->Attribute("name"));
						if (sharedAnimationMap.find(animationName) == sharedAnimationMap.end()) {
							DebugMsg(" - WARNING: animation with name '%s' not found!", animationName.c_str());
							continue;
						}

						vector<K70RGB> myAnimation = sharedAnimationMap.find(animationName)->second;
						for (unsigned int i = 0; i < myAnimation.size(); i++) {
							myColors.push_back(myAnimation[i]);
						}
					}

					
					for (TiXmlElement* baAColorNode = baANode->FirstChildElement("color"); baAColorNode != NULL; baAColorNode = baAColorNode->NextSiblingElement("color")) {
						K70RGB color;
						color.setR(atoi(baAColorNode->Attribute("r")));
						color.setG(atoi(baAColorNode->Attribute("g")));
						color.setB(atoi(baAColorNode->Attribute("b")));
						
						int duration = 0;
						if (baAColorNode->Attribute("duration")) {
							duration = atoi(baAColorNode->Attribute("duration"));
						}
						if (duration < 1) {
							duration = 1;
						}

						for (int i = 0; i < duration; i++) {
							myColors.push_back(color);
						}
						
					}
					DebugMsg(" - Added sharedBoardAnimations %s with duration of %i!", baName.c_str(), myColors.size());
					bAnim.addBoardAnimation(posX, posY, myColors);
					
				}
				sharedBoardAnimationMap.insert(make_pair(baName, bAnim));
			}
		}


	
		// iterate maps...
		for (TiXmlElement* mapNode = root->FirstChildElement("Map"); mapNode != NULL; mapNode = mapNode->NextSiblingElement("Map"))
		{
			
			bool activateOnKeyPresent = false;
			string mapname = string(mapNode->Attribute("name"));
			DebugMsg("Loaded Map: '%s'", mapname.c_str()); 
			ThemeMap myMap(mapname);
			
			// map switching
			for (TiXmlElement* mapConfigNode = mapNode->FirstChildElement(); mapConfigNode != NULL; mapConfigNode = mapConfigNode->NextSiblingElement())
			{
				string tagName = string(mapConfigNode->Value());
				// keygroups!
				if (tagName == string("activateOnKey")) {
				
					// get groups
					int groupId = 0;
					// DebugMsg("Loaded Map: '%s'", mapname.c_str());
					for (TiXmlElement* keyGroupNode = mapConfigNode->FirstChildElement("group"); keyGroupNode != NULL; keyGroupNode = keyGroupNode->NextSiblingElement("group")) {
						
						vector<string> keysOfGroup;
						// DebugMsg(" - activateOnKey:", mapname.c_str());
						for (TiXmlElement* keyNode = keyGroupNode->FirstChildElement("key"); keyNode != NULL; keyNode = keyNode->NextSiblingElement("key")) {
							string keyName = string(keyNode->Attribute("keyname"));
							/*
							if (keyNode == keyNode->LastChild("key")) {
								DebugMsg(" - - '%s'", keyName.c_str());
							}
							else {
								DebugMsg(" - - '%s', ", keyName.c_str());
							}
							*/
							keysOfGroup.push_back(keyName);
						}
						activateOnKeyPresent = true;
						currentTheme->AddKeySwitchGroup(mapname, groupId++, keysOfGroup);
						DebugMsg("keysOfGroup size: %i", keysOfGroup.size());
						
					}
				} // tagName == string("activateOnKey"

				else { // (tagName == string("key") || tagName == string("all") || tagName == string("KeyGroup")) {

					// create a temp keyGroup, if we need one...
					vector <string> KeyGroup;

					string baseKeyName;
					if (tagName == string("all")) {
						baseKeyName = "";
						DebugMsg(" - Loaded KeyAnimation for all Keys", baseKeyName);
						KeyGroup.push_back(baseKeyName.c_str());
					}
					else if (tagName == string("Key")) {
						if (!mapConfigNode->Attribute("keyname")) {
							DebugMsg(" - WARNING: Attribute 'keyname' is missing for key!");
							continue;
						}
						baseKeyName = string(mapConfigNode->Attribute("keyname"));
						DebugMsg(" - Loaded KeyAnimation for Key", baseKeyName.c_str());
						KeyGroup.push_back(baseKeyName);
					}
					else if (tagName == string("KeyGroup")) {
						if (!mapConfigNode->Attribute("name")) {
							DebugMsg(" - WARNING: Attribute 'name' is missing for KeyGroup!");
							continue;
						}
						string keyGroupName = string(mapConfigNode->Attribute("name"));
						if (sharedKeyGroupMap.find(keyGroupName) == sharedKeyGroupMap.end()) {
							DebugMsg(" - WARNING: KeyGroup with name '%s' not found!", keyGroupName.c_str());
							continue;
						}
						KeyGroup = sharedKeyGroupMap[keyGroupName];
						DebugMsg(" - Loaded KeyAnimation for KeyGroup '%s'", keyGroupName.c_str());
					}
					else {
						DebugMsg(" - WARNING: Tag '%s' unknown.", tagName.c_str());
					}
					

					// running throu keygroups
					for (vector<string>::iterator it = KeyGroup.begin(); it != KeyGroup.end(); ++it) {
					
						string keyName = (*it);
						/*
						if (tagName == string("all")) {
							// do nothing
						}
						else {
							 DebugMsg(" - - Key: '%s'", keyName.c_str());
						}
						*/
						
					

						for (TiXmlElement* colorGroupNode = mapConfigNode->FirstChildElement("colorGroup"); colorGroupNode != NULL; colorGroupNode = colorGroupNode->NextSiblingElement("colorGroup"))
						{
							string type = string(colorGroupNode->Attribute("type"));

							// clear existing mapping...
							//if (tagName != string("all")) {
								myMap.ClearKeyMapColors(keyName, type);
							//}

							string syncName = string("");
							if (colorGroupNode->Attribute("syncTo")) {
								myMap.SetSyncNameToKeyMap(keyName, type, string(colorGroupNode->Attribute("syncTo")));
							}
							else { // remove older sync
								myMap.RemoveSync(keyName, type);
							}

							if (colorGroupNode->Attribute("join")) {
								myMap.SetJoinMode(keyName, type, string(colorGroupNode->Attribute("join")));
							}

							unsigned int colorGroupDuration = 0;




							// check all colors
							for (TiXmlElement* colorNode = colorGroupNode->FirstChildElement(); colorNode != NULL; colorNode = colorNode->NextSiblingElement()) {
								string colorNodeTagName = string(colorNode->Value());

								if (colorNodeTagName == string("color")) {
									K70RGB color;
									color.setR(atoi(colorNode->Attribute("r")));
									color.setG(atoi(colorNode->Attribute("g")));
									color.setB(atoi(colorNode->Attribute("b")));
									unsigned int duration = 0;
									if (colorNode->Attribute("duration")) {
										duration = atoi(colorNode->Attribute("duration"));
									}
									if (duration < 1) {
										duration = 1;
									}

									colorGroupDuration += duration;

									myMap.AddColorToKeyMap(keyName, type, color, duration);
								}
								else if (colorNodeTagName == string("animation")) {
									vector<K70RGB> myAnimation;
									if (!colorNode->Attribute("name")) {
										DebugMsg(" - WARNING: Attribute 'name' is missing for animation!");
										continue;
									}
									string sharedAnimationName = string(colorNode->Attribute("name"));
									if (sharedAnimationMap.find(sharedAnimationName) == sharedAnimationMap.end()) {
										DebugMsg(" - WARNING: animation with name '%s' not found!", sharedAnimationName.c_str());
										continue;
									}
									
									string animationName = string(colorNode->Attribute("name"));
									DebugMsg(" - Loading shared Animation with name '%s'", animationName.c_str());
									myAnimation = sharedAnimationMap.find(animationName)->second;
									

									int startPoint = 0;
									// DebugMsg("Animation Size %i\n", myAnimation.size());
									if (colorNode->Attribute("offset")) {
										startPoint = atoi(colorNode->Attribute("offset"));
									}

									// DebugMsg("Animation StartPoint %i\n", startPoint);
									for (unsigned int i = startPoint; i < myAnimation.size() + startPoint; i++) {
										int j = i % myAnimation.size();
										// DebugMsg("Animation Read %i %i\n", i, j);
										myMap.AddColorToKeyMap(keyName, type, myAnimation[j], 1);
										colorGroupDuration++;
									}
								}
								else {
									DebugMsg(" - WARNING: TagName '%s' not recognized!", colorNodeTagName.c_str());
								}

							} // end all colors in group


							if (colorGroupNode->Attribute("syncName")) {
								currentTheme->AddSyncGroup(string(colorGroupNode->Attribute("syncName")), colorGroupDuration);
							}

							// boardAnimations
							if (colorGroupNode->Attribute("boardAnimation")) {
								string boardAnimationName = string(colorGroupNode->Attribute("boardAnimation"));
								DebugMsg(" - Added boardAnimation '%s'!", boardAnimationName.c_str());
								myMap.SetBoardAnimationName(keyName, type, boardAnimationName);
								myMap.addBoardAnimation(sharedBoardAnimationMap[boardAnimationName]);
							}


						} // colorNodes
					} // kgc++
					

				}
				
			}
			

			// add an empty activateOnKey
			if (!activateOnKeyPresent) {
				vector<string> keysOfGroup;
				currentTheme->AddKeySwitchGroup(mapname, 0, keysOfGroup);
			}
			

			currentTheme->AddMap(myMap);
			
			

		} // each map

	}
	else {
		DebugMsg("ThemeFile %s not found!", themeFileName.c_str());
	}

	themeName = localThemeName;
	currentTheme->GetMapByName(string("default"));
	

}



void K70XMLConfig::saveConfig() {
	
	DebugMsg("--------- SAVE CONFIG ------------");
	TiXmlDocument doc("config.xml");

	if (doc.LoadFile())
	{
		
		
		
		saveConfigValue(&doc, "Layout", getLayoutName());
		saveConfigValue(&doc, "Theme", getThemeName());

		if (IsWindowVisible(ghDlgMain)) {
			RECT Rect;
			GetWindowRect(ghDlgMain, &Rect);
			saveConfigValue(&doc, "WindowPositionTop", to_string(Rect.top));
			saveConfigValue(&doc, "WindowPositionLeft", to_string(Rect.left));
		}
		

	}





	doc.SaveFile("config.xml");
	
}


void K70XMLConfig::saveConfigValue(TiXmlDocument * doc, string tagName, string content) {
	TiXmlHandle handle(doc);

	TiXmlHandle configRoot = handle.FirstChild("Config");
	TiXmlElement * configRootElement = configRoot.ToElement();

	// check if the config tag is there...
	TiXmlElement * configElement = configRoot.FirstChild(tagName.c_str()).ToElement();
	
	// if not - go create one!
	if (!configElement) {
		TiXmlElement * element = new TiXmlElement(tagName.c_str());
		TiXmlText *blankText = new TiXmlText(string("").c_str());
		element->LinkEndChild(blankText);
		configRootElement->LinkEndChild(element);
		configElement = element;
	}
	
	// not it is there!
	TiXmlNode * configNode = configRoot.FirstChild(tagName.c_str()).FirstChild().ToNode();
	// set value!
	configNode->SetValue(content.c_str());
	

}