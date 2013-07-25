#pragma strict

import System.Text;
import System.Xml;
import System.Security;
import System.Collections.Generic;

var backgroundTexture : Texture2D;
var fontTexture : Texture2D;
var fontFile : TextAsset;

private var state : boolean;
private var texture : Texture2D;
private var windowWidth : uint;
private var windowHeight : uint;
private var lineHeight : uint;
private var lineOffset : int;
private var fontDocument : XmlDocument;
private var fontCharacters : FontCharacter[]; //length=127; Corresponds to ASCII IDs; Constant Time Access
private var cursor : Vector3;
private var colors : Dictionary.<String,Color>;
private var buffer : String; //new text being typed in the current block

class FontCharacter {
	var id : int;
	var x : int;
	var y : int;
	var width : int;
	var height : int;
	var xoffset : int;
	var yoffset : int;
	var xadvance : int;
	var page : int;
	var chnl : int;
	var colors : Color[];
	
	function FontCharacter() {
		this.id = 0; 
		this.x = 0;
		this.y = 0;
		this.width = 0;
		this.height = 0;
		this.xoffset = 0;
		this.yoffset = 0;
		this.xadvance = 0;
		this.page = 0;
		this.chnl = 0;
		this.colors = new Color[3];
	}
};



function LoadFont() {
	fontDocument.LoadXml(fontFile.text);
	
	var charNodeList : XmlNodeList = fontDocument.GetElementsByTagName("char");
	for(var n in charNodeList) {
		var fontCharacter = new FontCharacter();
		var charNode : XmlNode = n as XmlNode;
		if(!(int.TryParse(charNode.Attributes["id"].Value, fontCharacter.id)
			&&	int.TryParse(charNode.Attributes["x"].Value, fontCharacter.x)
			&&	int.TryParse(charNode.Attributes["y"].Value, fontCharacter.y)
			&&	int.TryParse(charNode.Attributes["width"].Value, fontCharacter.width)
			&&	int.TryParse(charNode.Attributes["height"].Value, fontCharacter.height)
			&&	int.TryParse(charNode.Attributes["xoffset"].Value, fontCharacter.xoffset)
			&&	int.TryParse(charNode.Attributes["yoffset"].Value, fontCharacter.yoffset)
			&&	int.TryParse(charNode.Attributes["xadvance"].Value, fontCharacter.xadvance)
			&&	int.TryParse(charNode.Attributes["page"].Value, fontCharacter.page)
			&&	int.TryParse(charNode.Attributes["chnl"].Value, fontCharacter.chnl))) {
				throw new System.ArgumentException("ConsoleScript: Parsing .fnt error.");	
		}
		fontCharacter.colors = fontTexture.GetPixels(fontCharacter.x, fontTexture.height - (fontCharacter.y+fontCharacter.height), fontCharacter.width, fontCharacter.height); //in .fnt, top-right is (0,0)

		fontCharacters[fontCharacter.id] = fontCharacter; //store for later use on textures
	}
}

function LoadTexture() {
	renderer.material.mainTexture = texture; //TODO: Set Turing Image as Console Background
	texture.SetPixels(0,0, windowWidth, windowHeight, backgroundTexture.GetPixels());
}

function NewLine() {
	cursor.x = 0.0;
	cursor.y -= lineHeight; //new row
}

function ApplyToTexture(parentNode : XmlNode, drawHeading : boolean, innerText : String) {

	var messageBytes : byte[] = Encoding.ASCII.GetBytes(innerText);
			
	for(var b : uint = 0; b < messageBytes.Length; b++) {
		var currentFontChar : FontCharacter = fontCharacters[messageBytes[b]];
		if(currentFontChar != null) {
				
			var placeX : int = cursor.x + currentFontChar.xoffset;	
				
			if(placeX+currentFontChar.width >= windowWidth) { //new row after each block of data, or on overflow
				NewLine();
				placeX = cursor.x + currentFontChar.xoffset;
		
			}
			
			var placeY : int = cursor.y + currentFontChar.yoffset + lineOffset*lineHeight; //correct; bottom-left is (0,0);
		
			if(drawHeading) { 
				placeY = texture.height-lineHeight;
			} else if(placeY >= (texture.height-lineHeight)) { //stop any line from rendering on the area of the title
				placeY = texture.height;
			}
			
			if(placeX < 0) { 
				placeX = 0;
			}
			
			if(Rect(0,0,texture.width,texture.height).Contains(Vector2(placeX,placeY))) {
				var tempColors : Color[] = new Color[currentFontChar.colors.length];
				System.Array.Copy(currentFontChar.colors, tempColors, currentFontChar.colors.length);
				var compareColors : Color[] = backgroundTexture.GetPixels(placeX, placeY, currentFontChar.width, currentFontChar.height);
			
							
				for(var c : uint = 0; c < currentFontChar.colors.length; c++) {
					if(currentFontChar.colors[c] == Color.black) { //color wallpaper first
						tempColors[c] = compareColors[c];
					} else if(parentNode.Name == "color") {
						var foreground : XmlAttribute = parentNode.Attributes["name"];
						
						if(foreground != null) { //color text third
							if(colors.ContainsKey(foreground.Value)) {
								tempColors[c] = colors[foreground.Value];
							}
						}
					}
				}
				texture.SetPixels(placeX, placeY, currentFontChar.width, currentFontChar.height, tempColors);
			} 
			
			cursor.x += currentFontChar.xadvance;
		}  
	}

}

function Recurse(xmlNode : XmlNode) {
	Recurse(xmlNode, false);
}

function Recurse(xmlNode : XmlNode, drawHeading : boolean) { //depth-first procedure taken from stack overflow
	if(xmlNode instanceof XmlElement) {
		
		if(xmlNode.Name == "heading") {
			drawHeading = true;
		}
		
		if(xmlNode.Name == "block") {
			NewLine();
			drawHeading = false; 
		}
		
		if(xmlNode.HasChildNodes) {
			Recurse(xmlNode.FirstChild, drawHeading);
		} 
		
		if(xmlNode.NextSibling != null) {
			Recurse(xmlNode.NextSibling, drawHeading);
		}
		
	} else if(xmlNode instanceof XmlText) {
		ApplyToTexture(xmlNode.ParentNode, drawHeading, (xmlNode as XmlText).Value); //TODO: Apply color by name from parent node
	}
}

function LoadConsole(xmlData : String) {
	LoadTexture();
	cursor = Vector3.zero;
	cursor.y = windowHeight;
	
	var xmlDocument : XmlDocument = new XmlDocument();
	xmlDocument.LoadXml(xmlData);
	Recurse(xmlDocument.DocumentElement);
	
	texture.Apply(false);
}

function LoadConsole() {
	if(state) {
		LoadConsole("<console><heading><color name='green'>ESC</color> | edit</heading><block>"+buffer+"</block></console>");
	} else {
		LoadConsole("<console></console>");
	}
}

function Start () {
	state = false;
	fontCharacters = new FontCharacter[127]; //Size of regular ascii characters
	windowWidth = 256;
	windowHeight = 256;
	lineHeight = 32;
	lineOffset = -1;
	colors = new Dictionary.<String,Color>();
	colors["red"] = Color.red;
	colors["white"] = Color.white;
	colors["green"] = Color.green;
	fontDocument = new XmlDocument();
	texture = new Texture2D(windowWidth,windowHeight, TextureFormat.RGB24, false);
	cursor = Vector3.zero;
	buffer = "";

	LoadFont();
	LoadConsole("<console></console>");
}

function OnControlsFocus(s : boolean) {
	state = s;
	if(state) {
		Camera.main.SendMessage("OnControlsFocus", false);
	} 
	LoadConsole();
}



function Update() {
	if(state) {
		
		if(Input.GetKeyDown(KeyCode.UpArrow)) {
			lineOffset--;
			LoadConsole();
		} else if(Input.GetKeyDown(KeyCode.DownArrow)) {
			lineOffset++;
			LoadConsole();
		} else {
		
			for(var c : char in Input.inputString) {
				if(c == "\b"[0]) {
					if(buffer.Length > 0) {
						buffer = buffer.Remove(buffer.Length-1, 1);
					}
				}else if(c == "\n"[0] || c == "\r"[0]) {
					
				} else {
					buffer += c;
				}
			}
			
			if(Input.inputString.Length > 0) {
				LoadConsole();
			}
		}
	}
}
