//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).

using UnityEngine;
using System.Collections;

public class RotateCameraScript : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		float damping = 2.0f;
		transform.Rotate(transform.localEulerAngles.y, 0, damping * Time.deltaTime);
	}
}
