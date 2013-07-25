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