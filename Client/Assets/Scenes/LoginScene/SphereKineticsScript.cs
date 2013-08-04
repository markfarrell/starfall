//Copyright (c) 2013 Mark Farrell

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
