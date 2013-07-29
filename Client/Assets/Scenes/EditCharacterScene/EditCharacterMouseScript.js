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

var mousePosition : Vector3 = Vector3.zero;
var rate : float = 180.0 / 1.0; //degrees per second

function Start () {

}

function Update () {

	if(Input.GetMouseButton(2))
	{
		var m = Input.mousePosition;
		
		if(m != mousePosition)
		{
			var d = m - mousePosition;
			
			var rotateVector : Vector3 = Vector3.zero;
			
			if(d.x < 0)
			{
				rotateVector += Vector3.up * (rate)*Time.deltaTime;
			}
			else if(d.x > 0)
			{
				rotateVector += Vector3.up * -(rate)*Time.deltaTime;
			}
			
			transform.Rotate(rotateVector.y*Vector3.up, Space.Self);

			mousePosition = m;
		}
	}

}