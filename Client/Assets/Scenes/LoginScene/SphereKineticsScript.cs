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


using UnityEngine;
using System.Collections;

public class SphereKineticsScript : MonoBehaviour {
	
	void Start() {
		transform.RotateAround(Vector3.zero, Vector3.up, -90.0f);	
	}
	// Update is called once per frame
	void Update () {
		
		//We can use the time since level load or differentiate the equations to use delta time;
		//transform.Translate( ( X(Time.timeSinceLevelLoad)*Vector3.right + Z(Time.timeSinceLevelLoad)*Vector3.forward) );
		
		//Rotate the mesh itself along its own axis
		transform.GetChild(0).Rotate(1.0f * Vector3.forward);
		transform.RotateAround(Vector3.zero, Vector3.up, 3f * Time.deltaTime);

	}
}
