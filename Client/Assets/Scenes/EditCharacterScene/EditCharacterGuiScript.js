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


//TODO: Implement file IO
import System;
import System.IO;
import System.Text; //to Write to the File as Text

import GameClient;
import Send;

/**
 * The GUI for the Edit Character Screen.
 * It should be possible to change the main material colour of each part
 * on the character. 
 */
 
 var skin : GUISkin;
 
 var playerGameObject : GameObject;
 /** 
  * The materials should be selected with the inspector. 
  */
 var helmetMaterial    : Material;
 var visorMaterial     : Material;
 var visorTrimMaterial : Material;
 var bodyMaterial      : Material;
 var torsoMaterial     : Material;
 var rightLegMaterial  : Material;
 var rightFootMaterial : Material;
 var rightArmMaterial  : Material;
 var rightHandMaterial : Material;
 var leftLegMaterial   : Material;
 var leftFootMaterial  : Material;
 var leftArmMaterial   : Material;
 var leftHandMaterial  : Material;
 
 
//The helmet, body, arms and legs can use the standard colors. 
//The color options are different for gloves, boots and wrists
 var standardColors : EditCharacterColor[];

 
 private var username : String = " ";
 private var parts : EditCharacterItem[];
 
 private var showSaveDialog : boolean = false;
 private var saveFileDestination : String = "";
 
 
 class EditCharacterColor {
 
 	var name : String;
 	var color : Color;
 	
 	function EditCharacterColor( n : String, c : UnityEngine.Color ) {
 		this.name = n;
 		this.color = c;
 	}
 	
 };
 
 

 class EditCharacterItem {
 

 	var name : String;
 	var material : Material; //the material ready to be applied to the mesh
 	var renderedMaterial : Material; //a reference to the material that can be updated on the mesh
 	var originalMaterial : Material; //a copy that acts as a backup; for resetting the material color on destruction

 	var replacePartName : String;
 	var replaceMaterialName : String;
 	var selectedColor : int;
 	
 	function EditCharacterItem(n : String, rp : String, rm : String, m : Material) {

 		this.name = n;
 		this.replacePartName = rp;
 		this.replaceMaterialName = rm;
 		this.originalMaterial = new Material(m);
 		this.material = new Material(m); //instantiate the material
 		this.selectedColor = 0;
 	}
 	
 	
	function ApplyMaterialColor(c : Color){
		this.material.color = c;
		if(this.renderedMaterial != null){
			this.renderedMaterial.color = this.material.color;
		}
	}
	
 	function ResetMaterial() {
 		if(this.renderedMaterial != null) {
 			Debug.Log("Resetting the material for " + this.name + " to " + this.replaceMaterialName );
 			this.renderedMaterial.color = this.originalMaterial.color;
 		}
 	}
 	
 };
 
 
 /*
  * Reset to the original material when the script is destroyed.
  */
 function OnDestroy() {
 	for(var part : EditCharacterItem in parts) {
 		if(part != null) {
			part.ResetMaterial();
		}
	}
	Program.Client.End();
 }
 
 function Start() {
 		parts = [ EditCharacterItem("Helmet",         "Helmet",    "MaterialHelmet",   helmetMaterial), 
				  EditCharacterItem("Visor",          "Helmet",     "MaterialVisor",    visorMaterial),
				  EditCharacterItem("Trim",           "Helmet", "MaterialVisorEdge", visorTrimMaterial),
				  EditCharacterItem("Body",             "Body",      "MaterialBody",      bodyMaterial),
				  EditCharacterItem("Torso",           "Torso",     "MaterialTorso",     torsoMaterial), 
				  EditCharacterItem("Right Leg",   "Right_Leg",  "RightLegMaterial",  rightLegMaterial),
				  EditCharacterItem("Right Foot", "Right_Foot", "RightFootMaterial", rightFootMaterial), 
				  EditCharacterItem("Right Arm",   "Right_Arm",   "RightArmMainMaterial",  rightArmMaterial),
				  EditCharacterItem("Right Hand",  "Right_Hand", "RightHandMaterial", rightHandMaterial),
				  EditCharacterItem("Left Leg",     "Left_Leg",    "LeftLegMaterial",   leftLegMaterial),
				  EditCharacterItem("Left Foot",   "Left_Foot",   "LeftFootMaterial", leftFootMaterial),
				  EditCharacterItem("Left Arm",     "Left_Arm",    "LeftArmMainMaterial", leftArmMaterial),
				  EditCharacterItem("Left Hand",   "Left_Hand",   "LeftHandMaterial", leftHandMaterial)];


	/**TODO: Assign mesh renderers here. */

		for(var component in playerGameObject.transform) {
			var playerChildObject : Transform = component as Transform;
			
			for(var part : EditCharacterItem in parts) {		  
	
			if(part.replacePartName == playerChildObject.name) {
				Debug.Log("Matched Component: " + playerChildObject.name);
				
				
				var meshRenderer : MeshRenderer = playerChildObject.GetComponent(MeshRenderer);
				if(meshRenderer != null) {
					/**
					 * WARNING: Do not use "render.materials" because unity will instantiate 
					 * the materials and replace existing references on the mesh. Then, they 
					 * will be destroyed and the models materials will appear corrupted! Use
					 * "renderer.sharedMaterials" instead.
					 */
					var n = meshRenderer.sharedMaterials.length;
					Debug.Log("Found mesh renderer for: " + part.name + " with " + n + " materials.");
						
					for(var mat : Material in meshRenderer.sharedMaterials) {
						if(part.replaceMaterialName == mat.name) {
							Debug.Log("Matched material on: " + playerChildObject.name + " with name " + mat.name);
							part.renderedMaterial = mat;
						}
					}
				}
			}
		}
	}
	
 }
 
 
function OnGUI () {

	GUI.skin = skin;
			  		  		  
	GUI.BeginGroup( Rect(0, (Screen.height - 800) / 2, 200, 800) );
	
	if(parts != null) {
	
		for( var i = 0; i < parts.length; i++) {	
			var y = (24*2)*i;
			GUI.Label(Rect(10, y, 100, 24), parts[i].name);
			
				
			if(standardColors.length > 0)
			{			
				if(GUI.Button(Rect(10, y+24, 30,24), "<")){
					 if(parts[i].selectedColor-1 > -1){
						parts[i].selectedColor--;
					}
				}
					
				if(GUI.Button(Rect(40, y+24, 100,24), standardColors[parts[i].selectedColor].name)){
					if(parts[i].selectedColor != -1){
						parts[i].ApplyMaterialColor(standardColors[parts[i].selectedColor].color); 
					}					
				}
					
				if(GUI.Button(Rect(140, y+24, 30,24), ">")) {
					if(parts[i].selectedColor+1 < standardColors.length) {
						parts[i].selectedColor++;
					}
				}
				
		 	}
		}
	}
		
	GUI.EndGroup();
	
	GUI.BeginGroup( Rect( (Screen.width/2) - 40 - (100/2) , (Screen.height - 100)/2 + 100, 200, 100) );

		GUI.Label(Rect(0,0,100,24), "Name:");
		username = GUI.TextField(Rect(40,0,100,24),username, 12);
		
	GUI.EndGroup();
	
	
	GUI.BeginGroup(Rect((Screen.width-60)/2,20,60,40));
		if(GUI.Button(Rect(0,0,50,30), "Save")) {	
			showSaveDialog = true;
		}
	GUI.EndGroup();
	
	
	if(showSaveDialog) {
		//TODO: Only make this box appear when "Save" is pressed.
		GUI.BeginGroup(Rect((Screen.width-300)/2, (Screen.height-300)/2 - 75,300,300));
			
			GUI.Box(Rect(0,0,300,300), "Save Dialog");
			GUI.Label(Rect((300-(90+100))/2,40,90,30), "Name (.json): "); //file name
			saveFileDestination = GUI.TextField(Rect((300-(90+100))/2 + 80, 40, 100, 30), saveFileDestination, 30);
			if( GUI.Button(Rect( (300-(70+70)) /2 ,80,70,30), "OK") ) {
				//file io
				
				var path = "./Data/" + saveFileDestination + ".json";
				
				if(saveFileDestination != "")
				{
					
					var AddText = function( s : String, fs : FileStream ) {
						//Taken from MSDN
						var info : byte[] = new UTF8Encoding(true).GetBytes(s);
						fs.Write(info, 0, info.Length);
					};
					
					//Taken from MSDN
					if (File.Exists(path))
        			{
            			File.Delete(path);
        			}
        			  
					var fileStream : FileStream = File.Create(path);
					
					var FindPart = function ( name : String ) { 
						for(var part : EditCharacterItem in parts) {
							if(part.name == name) { 
								return part;
							}
						}
						return null;
					};
					
					var headGroup = [FindPart("Helmet"), FindPart("Visor"), FindPart("Trim")];
					var topGroup = [FindPart("Body"), FindPart("Left Arm"), FindPart("Right Arm")];
					var bottomGroup = [FindPart("Torso"), FindPart("Left Leg"), FindPart("Right Leg")];
					var handsGroup = [FindPart("Left Hand"), FindPart("Right Hand")];
					var feetGroup = [FindPart("Left Foot"), FindPart("Right Foot")];
					
					var builder : StringBuilder = new StringBuilder();
					
					var BuildGroup = function( groupName: String, group : EditCharacterItem[], builder : StringBuilder) {
						var p = 0;
						builder.Append("\""+groupName+"\"")
							   .Append(":")
						       .Append("{") //begin group
						       .Append("\"type\":\"group\"")
						       .Append(",")
						       .Append("\""+"content"+"\"")
						       .Append(":")
						       .Append("{"); //begin content
						for(var part : EditCharacterItem in group) {
							builder.Append("")
								   .Append("\""+part.name+"\"")
								   .Append(":")
								   .Append("{")
								   .Append("\"type\" : ")
								   .Append("\"color\"")
								   .Append(", ")
								   .Append("\"red\" : ")
								   .Append("\""+part.material.color.r+"\"")
								   .Append(", ")
								   .Append("\"green\" : ")
								   .Append("\""+part.material.color.g+"\"")
								   .Append(", ")
								   .Append("\"blue\" : ")
								   .Append("\""+part.material.color.b+"\"")
								   .Append("}");
								if(p != group.length-1) {
									builder.Append(", ");
								}
								p++;
						}
						builder.Append("}"); //end content
						builder.Append("}"); //end group
					};
					
					builder.Append("{")
							.Append("\"head\" : { ") //Begin Head
							.Append("\"name\" : \"Parts\"") 
							.Append(",")
							.Append("\"datetime\" : \"--\" }") //End Head
							.Append(",")
							.Append("\"body\" : { ");
					BuildGroup("head", headGroup, builder);
					builder.Append(",");
					BuildGroup("top", topGroup, builder);
					builder.Append(",");
					BuildGroup("bottom", bottomGroup, builder);
					builder.Append(",");
					BuildGroup("hands", handsGroup, builder);
					builder.Append(",");
					BuildGroup("feet", feetGroup, builder);
		

					builder.Append("}")
							.Append(",")
							.Append("\"foot\" : {}")
							.Append("}");
					AddText(builder.ToString(), fileStream);
					
				
					Program.Client.player.user.consoleQueue.push(builder.ToString());
					Program.Client.send.Enqueue(Program.Client.player, Program.Client.send.ConsoleData);
					Program.Client.SendPackets();
					
					
					Debug.Log("Success writing file: " + saveFileDestination);
					fileStream.Close();
				}
				
			}
			if( GUI.Button( Rect( (300-(70+70))/2 + 70 + 5, 80, 70, 30), "Cancel") ) {
				showSaveDialog = false;	
				//TODO: Also "reset/destroy" any operations in place
				saveFileDestination = "";
			}	
		GUI.EndGroup();
		
	}
	
}






