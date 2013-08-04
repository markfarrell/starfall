//Copyright (c) 2013 Mark Farrell



#pragma strict

function Update () {
	this.transform.position.x = ((Screen.width-this.guiTexture.texture.width)/2.0)/Screen.width;
	this.transform.position.y = ((Screen.height-this.guiTexture.texture.height)/2.0+this.guiTexture.texture.height)/Screen.height;
}