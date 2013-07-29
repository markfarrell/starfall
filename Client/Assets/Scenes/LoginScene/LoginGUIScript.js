//The MIT License (MIT)
//
//Copyright (c) 2013 Mark Farrell
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.


#pragma strict

import System.Net.Sockets;
import System.Text;
import GUIStruct; //Network.js
import GameClient; //Network.js
import Program; //Network.js

/* Editor Variables */
public var skin : GUISkin;
public var address : String;
public var port : int;


private var guiStruct : GUIStruct;


function Start () {
	guiStruct = new GUIStruct();
	Program.Client = new GameClient(address,port);
}

function OnGUI() {
		
		GUI.skin = skin;
		GUI.BeginGroup(Rect((Screen.width-512)/ 2, (Screen.height-128)/2,512,256));
		GUI.Box(Rect(0,0,512,256), "");
		
		GUI.Label(Rect(60,(256-28)/2-20, 100, 28), guiStruct.userLabelString);
		GUI.Label(Rect(60,(256-28)/2+20,100, 28), guiStruct.passwordLabelString);
		
		guiStruct.userFieldString = GUI.TextField(Rect(150,(256-28)/2-20,150,28), guiStruct.userFieldString, 12);
		guiStruct.passwordFieldString = GUI.TextField(Rect(150,(256-28)/2+20,150,28), guiStruct.passwordFieldString, 12);

		
		//Centered horizontally; Vertical spacing: 10 units
		if(GUI.Button(Rect(350, (256-128)/2, 128, 128), "Connect")){ 
			guiStruct = Program.Client.Init(guiStruct);
			Program.Client.End();
		}
		
		GUI.Label(Rect(60,(256-28)/2+20+40,300,28), guiStruct.message); //Vertical spacing: 10 units
		
		GUI.EndGroup();
}

