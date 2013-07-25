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

