//Copyright (c) 2013 Mark Farrell -- This source code is released under the MIT license (http://opensource.org/licenses/MIT).

#pragma strict

import Player;
import User;
import Send;
import CreateEntityStruct;
import DestroyEntityStruct;
import GameClient;
import Program;

var topJetMaterial : Material;
var bottomJetMaterial : Material;
var entityPrefab : Transform;
var emptyPrefab : Transform;
var headlightPrefab : Transform; 

private var world : World;

class Config {
	static function PathTime() {
		return 0.100;
	}
	static function SendTime() {
		return 0.500;
	}
}

class Timer {

	var current : float;
	var limit : float;
	
	function Timer(limit : float) {
		this.limit = limit;
		this.Reset();
	}
	
	function Down() {
		this.current -= Time.deltaTime;
		if(this.current < 0.0) {
			this.current = 0.0;
		}
	}
	
	function IsDone() {
		return (this.current <= 0.0);
	}
	
	function Reset() {
		this.current = this.limit;
	}
	
	function Update(doneFunc : Function) {
		this.Down();
		if(this.IsDone()) {
			doneFunc();
			this.Reset();
		}
	}
	
	
};

class Lean {
	var leanAngle  : float; //max lean angle (rotation on X axis)
	var leanRate : float; //change in lean angle (degrees) per second when leaning forward
	var leanQuaternion : Quaternion;
	var originalQuaternions : Quaternion[]; //revert the leaning rotation after it is finished
	var maxQuaternions : Quaternion[]; //upper limit for the leaning rotation
		
	function Lean() {
		this.leanAngle = 20.0;
		this.leanRate = 30.0 / 1.0;
		this.leanQuaternion = Quaternion.identity;
	}
		
	function Begin(model:Transform) { 
		var originals = new Array();
		var maximums = new Array();
		for(var component in model.transform) { 
			var t = component as Transform;
			var localRotation = t.transform.localRotation;
			originals.push(new Quaternion(localRotation.x,localRotation.y,localRotation.z,localRotation.w));
			localRotation.eulerAngles.x -= this.leanAngle;
			maximums.push(new Quaternion(localRotation.x,localRotation.y,localRotation.z,localRotation.w));
		}
		this.originalQuaternions = originals.ToBuiltin(Quaternion) as Quaternion[];
		this.maxQuaternions = maximums.ToBuiltin(Quaternion) as Quaternion[];
		this.leanQuaternion.eulerAngles.x = 0.0;
	}
		
	function Forward(model : Transform) { 
		if(this.leanQuaternion.eulerAngles.x < this.leanAngle) {
			var deltaAngle : float = this.leanRate*Time.deltaTime;
			var next : float = this.leanQuaternion.eulerAngles.x+deltaAngle; 
			if(next > this.leanAngle) {
				this.leanQuaternion.eulerAngles.x = this.leanAngle;
				var i = 0;
				for(var component in model.transform) { 
					var t : Transform = component as Transform;
					t.transform.localRotation = this.maxQuaternions[i];
					i++;
				}
			} else {
				this.leanQuaternion.eulerAngles.x += deltaAngle;
				for(var component in model.transform) { 
					var t2 : Transform = component as Transform;
					t2.transform.Rotate(Vector3(-deltaAngle,0.0,0.0), Space.Self);
				}
			}
		}
	}
		
	function Back(model : Transform) { 
		var deltaAngle : float = this.leanRate*Time.deltaTime;
		var next : float = this.leanQuaternion.eulerAngles.x-deltaAngle;
		if(next < 0.0) { 
			this.leanQuaternion.eulerAngles.x = 0.0;
			var i = 0;
			for(var component in model.transform) { 
				var t : Transform = component as Transform;
				t.transform.localRotation = this.originalQuaternions[i];
				i++;
			}
		} else {
			this.leanQuaternion.eulerAngles.x -= deltaAngle;
			for(var component in model.transform) { 
				var t2 : Transform = component as Transform;
				t2.transform.Rotate(Vector3(deltaAngle,0.0,0.0), Space.Self);
			}
		}
	}
		
	function Ended() { 
		return !(this.leanQuaternion.eulerAngles.x > 0.0);
	}
		
	function Reset() { 
		this.leanQuaternion = Quaternion.identity;
	}
};
	

class CameraController {

	var parent : Controls;

	var mousePosition : Vector3;
	var rotateRate : float;
	var rotateVector : Vector3;

	var distanceVector : Vector3; 
	var minDistanceZ : float;
	var maxDistanceZ : float;
	var distanceRate : float; //units per input; discrete

	var originalCameraPosition : Vector3 = Vector3.zero;
	var originalCameraRotation : Quaternion = Quaternion.identity;
	
	function CameraController(parent:Controls) {
		this.parent = parent;
			  
		this.mousePosition = Vector3.zero;
		this.rotateRate = 180.0 / 1.0; //degrees per second
		this.rotateVector = Vector3.zero;
		
		this.distanceVector = Vector3(0.0,-0.04,0.1);
		this.minDistanceZ = 0.1;
		this.maxDistanceZ = 0.2;
		this.distanceRate = 0.02;
		
		this.originalCameraPosition = Camera.main.transform.position;
		this.originalCameraRotation = Camera.main.transform.rotation;
	}
	
	
	function Apply(model : Transform, view : View) { 
		var rotationQuaternion = model.rotation*Quaternion.Euler(this.rotateVector);
		Camera.main.transform.rotation = rotationQuaternion;
		Camera.main.transform.position = rotationQuaternion*(this.distanceVector*-1.0) + model.position;
	}
	
	function OnReset(model : Transform, view : View) { 
		this.rotateVector = Vector3.zero;
		this.distanceVector.z = this.minDistanceZ;
		this.Apply(model,view);
	}
	

	
	function OnRotation(model : Transform, view : View) {
			var m = Input.mousePosition;
			if(m != this.mousePosition)
			{
				var d = m - this.mousePosition;
				
				if(d.x < 0)
				{
					this.rotateVector += Vector3.up * (this.rotateRate)*Time.deltaTime;
				}
				else if(d.x > 0)
				{
					this.rotateVector += Vector3.up * -(this.rotateRate)*Time.deltaTime;
				}
				this.Apply(model,view);
				this.mousePosition = m;
			}
	}
	
	function OnZoom(model : Transform, view : View) { 
		var deltaAxis : float = Input.GetAxis("Mouse ScrollWheel");
		if(deltaAxis) {
			if(deltaAxis < 0) { 
				if((this.distanceVector.z+this.distanceRate) < this.maxDistanceZ) {
					this.distanceVector.z += (this.distanceRate);
				} 
			} else if(deltaAxis > 0) { 
				if((this.distanceVector.z-this.distanceRate) > this.minDistanceZ) {
					this.distanceVector.z -= (this.distanceRate);
				} 
			}
		this.Apply(model,view);
		}
	}
	
};

class PlayerController {

	var lean : Lean;
	var parent : Controls;
	var movementRate : float; //change in forward position per second
	var rotateRate : float; //change in degrees per second

	function PlayerController(parent : Controls) { 
		this.parent = parent;
		this.movementRate = 0.1 / 1.0;
		this.rotateRate = 180.0 / 1.0;
		this.lean = new Lean();
	}
	
	function TryRotate(vec : Vector3, model : Transform, view : View) {
		model.transform.Rotate(vec*(this.rotateRate*Time.deltaTime), Space.Self);
		this.ChangeVelocity(model,view);
	}
	
	function ChangeVelocity(model : Transform, view : View) {
		if(this.parent.states["forward"] == 1) { //only change the velocity vector if the player is already moving when rotating
			var moveVector : Vector3 = model.transform.rotation*Vector3(0.0,0.0,1.0);
			moveVector.Normalize();
			moveVector *= this.movementRate;
			var deltaVelocity : Vector3 = moveVector-model.rigidbody.velocity;
			model.rigidbody.AddForce(deltaVelocity, ForceMode.VelocityChange);
		}
		this.parent.camera.Apply(model,view);
	}

	function OnRightClick(model : Transform, view : View) {
		var ray : Ray = Camera.main.ScreenPointToRay(Input.mousePosition);
		var raycastHit : RaycastHit;
		
		if(Physics.Raycast(ray, raycastHit, 0.3)) { //allow players to align their view from a far distance away
			var angleBetween = Vector3.Angle(raycastHit.normal, ray.direction);
			var scalar : float = -1.0;
			if(angleBetween <= 90.0) { //angle is acute, look in opposite direction
				scalar = 1.0;
			}
			model.transform.rotation = Quaternion.LookRotation(Vector3.RotateTowards(model.transform.forward, scalar*raycastHit.normal, 1.0*Time.deltaTime, 1.0));
			this.ChangeVelocity(model,view);
		}
	
	}
	
	function OnLeftClick(model : Transform, view : View) {
		var ray : Ray = Camera.main.ScreenPointToRay(Input.mousePosition);
		var raycastHit : RaycastHit;

		if(Physics.Raycast(ray, raycastHit, 0.15)) { //allow players to click objects up close
			this.parent.SetCurrentOwner(raycastHit.collider);
		}
	}

	function OnBeginLeft(model : Transform, view : View) {
		this.parent.states["left"] = 1; //on->accelerate
	}
	
	function OnEndLeft(model : Transform, view : View) { 
		this.parent.states["left"] = 2;//on->decelerate
	}
	
	function OnBeginRight(model : Transform, view : View) {
		this.parent.states["right"] = 1; //on->accelerate
	}
	
	function OnEndRight(model : Transform, view : View) { 
		this.parent.states["right"] = 2;//on->decelerate
	}
	
	function OnBeginUp(model : Transform, view : View) {
		this.parent.states["up"] = 1; //on->accelerate
	}
	
	function OnEndUp(model : Transform, view : View) { 
		this.parent.states["up"] = 2;//on->decelerate
	}
	
	function OnBeginDown(model : Transform, view : View) {
		this.parent.states["down"] = 1; //on->accelerate
	}
	
	function OnEndDown(model : Transform, view : View) { 
		this.parent.states["down"] = 2;//on->decelerate
	}
	
	function OnBeginRollWest(model : Transform, view : View) {
		this.parent.states["rollwest"] = 1; //on->accelerate
	}
	
	function OnEndRollWest(model : Transform, view : View) { 
		this.parent.states["rollwest"] = 2;//on->decelerate
	}
	
	function OnBeginRollEast(model : Transform, view : View) {
		this.parent.states["rolleast"] = 1; //on->accelerate
	}
	
	function OnEndRollEast(model : Transform, view : View) { 
		this.parent.states["rolleast"] = 2;//on->decelerate
	}
	
	
	function OnBeginForward(model : Transform, view : View) {
		if(this.parent.states["forward"] == 0) {
			this.parent.states["forward"] = 1; //on->accelerate
			view.UpdateJets(this.parent.states["forward"]);
			this.lean.Begin(model);
			this.ChangeVelocity(model,view); //note: forward state must already be set to 1
		}
	}
	
	function OnEndForward(model : Transform, view : View) { 
		if(this.parent.states["forward"] == 1) {
			this.parent.states["forward"] = 2;//on->decelerate
			view.UpdateJets(this.parent.states["forward"]);
			model.rigidbody.velocity = Vector3.zero;
		}
	}
	
	/* Description: There is only one brake state.
	 * The user is either stopping their accelerating forward or they are not.
	 */
	function OnBrake(model : Transform, view : View) {
		this.parent.states["brake"] = 1; //on->brake
	}
	
	function ContinueBeginLeft(model : Transform, view : View) { 
		TryRotate(Vector3.down, model, view);
	}
	
	function ContinueEndLeft(model : Transform, view : View) { 
		this.parent.states["left"] = 0; //off
	}
	
	function ContinueBeginRight(model : Transform, view : View) { 
		TryRotate(Vector3.up, model, view);
	}
	
	function ContinueEndRight(model : Transform, view : View) { 
		this.parent.states["right"] = 0; //off
	}
	
	function ContinueBeginUp(model : Transform, view : View) { 
		TryRotate(Vector3.right, model, view);
	}
	
	function ContinueEndUp(model : Transform, view : View) { 
		this.parent.states["up"] = 0; //off
	}
	
	function ContinueBeginDown(model : Transform, view : View) { 
		TryRotate(Vector3.left, model, view);
	}
	
	function ContinueEndDown(model : Transform, view : View) { 
		this.parent.states["down"] = 0; //off
	}
	
	function ContinueBeginRollWest(model : Transform, view : View) { 
		TryRotate(Vector3.forward, model, view);
	}
	
	function ContinueEndRollWest(model : Transform, view : View) { 
		this.parent.states["rollwest"] = 0; //off
	}
	
	function ContinueBeginRollEast(model : Transform, view : View) { 
		TryRotate(Vector3.back, model, view);
	}
	
	function ContinueEndRollEast(model : Transform, view : View) { 
		this.parent.states["rolleast"] = 0; //off
	}
	
	
	function ContinueBeginForward(model : Transform, view : View) { 
		this.lean.Forward(model);
		this.parent.camera.Apply(model,view);
	}
	
	
	function ContinueEndForward(model : Transform, view : View) { 
	
		if(this.lean.Ended()) { 
			this.lean.Reset();
			this.parent.states["forward"] = 0; //off
			view.UpdateJets(this.parent.states["forward"]);
		} else { 
			this.lean.Back(model);
		} 
	}
	
	function ContinueBrake(model : Transform, view : View) { 
		this.parent.states["brake"] = 0; //off
	}
	
};

class Controls {

	var parent : World;
	
	var focussed : boolean;
	var currentOwner : Collider; //not null if another collider, such as a Console, owns the controls; otherwise null
	
	var camera : CameraController;
	var play : PlayerController;

	var states : Hashtable;
	var updates : Hashtable; 
	


	function Controls(parent : World) { //TODO: Add key layout to view class
		this.parent = parent;
		this.focussed = true;
		this.currentOwner = null;
		this.camera = new CameraController(this);
		this.play = new PlayerController(this);
		
		this.states = new Hashtable();  
		this.states["forward"] = 0; //off 
		this.states["brake"] = 0; //off
		this.states["left"] = 0; //off
		this.states["right"] = 0; //off
		this.states["up"] = 0; //off
		this.states["down"] = 0; //off
		this.states["rollwest"] = 0; //off
		this.states["rolleast"] = 0; //off
		this.updates = new Hashtable();
		
		//These updaters switch controls on and off
		this.updates[(function() { return Input.GetMouseButton(0) && this.focussed; }) as Function] = (this.play.OnLeftClick as Function);
		this.updates[(function() { return Input.GetMouseButton(1) && this.focussed; }) as Function] = (this.play.OnRightClick as Function);
		this.updates[(function() { return Input.GetMouseButton(2); }) as Function] = (this.camera.OnRotation as Function);
		this.updates[(function() { return Input.GetAxis("Mouse ScrollWheel"); }) as Function] = (this.camera.OnZoom as Function);
		this.updates[(function() { return Input.GetKeyDown(KeyCode.R) && this.focussed; }) as Function] = (this.camera.OnReset as Function);
		this.updates[(function() { return Input.GetKeyDown(KeyCode.A) && this.focussed; }) as Function] = (this.play.OnBeginLeft as Function);
		this.updates[(function() { return Input.GetKeyUp(KeyCode.A); }) as Function] = (this.play.OnEndLeft as Function);
		this.updates[(function() { return Input.GetKeyDown(KeyCode.D) && this.focussed; }) as Function] = (this.play.OnBeginRight as Function);
		this.updates[(function() { return Input.GetKeyUp(KeyCode.D); }) as Function] = (this.play.OnEndRight as Function);	
		this.updates[(function() { return Input.GetKeyDown(KeyCode.Q) && this.focussed; }) as Function] = (this.play.OnBeginUp as Function);
		this.updates[(function() { return Input.GetKeyUp(KeyCode.Q); }) as Function] = (this.play.OnEndUp as Function);	
		this.updates[(function() { return Input.GetKeyDown(KeyCode.E) && this.focussed; }) as Function] = (this.play.OnBeginDown as Function);
		this.updates[(function() { return Input.GetKeyUp(KeyCode.E); }) as Function] = (this.play.OnEndDown as Function);	
		this.updates[(function() { return Input.GetKeyDown(KeyCode.Z) && this.focussed; }) as Function] = (this.play.OnBeginRollWest as Function);
		this.updates[(function() { return Input.GetKeyUp(KeyCode.Z); }) as Function] = (this.play.OnEndRollWest as Function);	
		this.updates[(function() { return Input.GetKeyDown(KeyCode.C) && this.focussed; }) as Function] = (this.play.OnBeginRollEast as Function);
		this.updates[(function() { return Input.GetKeyUp(KeyCode.C); }) as Function] = (this.play.OnEndRollEast as Function);	
		this.updates[(function() { return Input.GetKeyDown(KeyCode.W) && this.focussed; }) as Function] = (this.play.OnBeginForward as Function);
		this.updates[(function() { return Input.GetKeyUp(KeyCode.W); }) as Function] = (this.play.OnEndForward as Function);
		this.updates[(function() { return Input.GetKeyDown(KeyCode.S) && this.focussed; }) as Function] = (this.play.OnBrake as Function);
		this.updates[(function() { return Input.GetKeyUp(KeyCode.Escape); }) as Function] = (this.OnRestoreFocus as Function);
		//These updaters continuously process while in a given state
		this.updates[(function() { return (this.states["left"]==1); }) as Function] = (this.play.ContinueBeginLeft as Function);
		this.updates[(function() { return (this.states["left"]==2); }) as Function] = (this.play.ContinueEndLeft as Function);
		this.updates[(function() { return (this.states["right"]==1); }) as Function] = (this.play.ContinueBeginRight as Function);
		this.updates[(function() { return (this.states["right"]==2); }) as Function] = (this.play.ContinueEndRight as Function);
		this.updates[(function() { return (this.states["up"]==1); }) as Function] = (this.play.ContinueBeginUp as Function);
		this.updates[(function() { return (this.states["up"]==2); }) as Function] = (this.play.ContinueEndUp as Function);
		this.updates[(function() { return (this.states["down"]==1); }) as Function] = (this.play.ContinueBeginDown as Function);
		this.updates[(function() { return (this.states["down"]==2); }) as Function] = (this.play.ContinueEndDown as Function);
		this.updates[(function() { return (this.states["rollwest"]==1); }) as Function] = (this.play.ContinueBeginRollWest as Function);
		this.updates[(function() { return (this.states["rollwest"]==2); }) as Function] = (this.play.ContinueEndRollWest as Function);
		this.updates[(function() { return (this.states["rolleast"]==1); }) as Function] = (this.play.ContinueBeginRollEast as Function);
		this.updates[(function() { return (this.states["rolleast"]==2); }) as Function] = (this.play.ContinueEndRollEast as Function);
		this.updates[(function() { return (this.states["forward"]==1); }) as Function] = (this.play.ContinueBeginForward as Function);
		this.updates[(function() { return (this.states["forward"]==2); }) as Function] = (this.play.ContinueEndForward as Function);
		this.updates[(function() { return (this.states["brake"]==1); }) as Function] = (this.play.ContinueBrake as Function);
	}
	
	function OnRestoreFocus(model : Transform, view : View) {
		this.focussed = true;
		if(this.currentOwner != null) {
			this.currentOwner.SendMessage("OnControlsFocus", false, SendMessageOptions.DontRequireReceiver);
			this.currentOwner = null;
		}
	}
	
	function Update() { 
		try 
		{
			for(var key in this.updates.Keys) { 
				var updateCriteriaFunc : Function = key as Function;
				var runUpdateFunc : Function = this.updates[updateCriteriaFunc] as Function;
				if(updateCriteriaFunc()){
					if(this.parent.entity != null) {
						runUpdateFunc(this.parent.entity.model,this.parent.entity.view);
					}
				}
			}
		}catch(e:NullReferenceException){
			Debug.Log(e);
		}
	}
	
	function SetCurrentOwner(owner : Collider) {
		if(this.currentOwner != null) {
			this.currentOwner.SendMessage("OnControlsFocus", false, SendMessageOptions.DontRequireReceiver); //end focus for current owner
		}	
		this.currentOwner = owner; 
		this.currentOwner.SendMessage("OnControlsFocus", true, SendMessageOptions.DontRequireReceiver); //start focus for new owner
	}

};

class View  {

	var parent : Entity;
	var originalTopJetMaterial : Material;
	var originalBottomJetMaterial : Material;
	var recolorTable : Hashtable;
	var state;
	
	class Indices {
		var meshRendererIndex : uint;
		var materialIndex : uint;
		function Indices(meshRendererIndex : uint, materialIndex : uint) { 
			this.meshRendererIndex = meshRendererIndex;
			this.materialIndex = materialIndex;
		}
	};
	
	function View(parent : Entity, topJetMaterial : Material, bottomJetMaterial : Material) { 
		this.parent = parent;
		this.recolorTable = new Hashtable();
		this.originalTopJetMaterial = topJetMaterial;
		this.originalBottomJetMaterial = bottomJetMaterial;
		this.state = 0;
		this.recolorTable.Clear();
		this.Register(this.originalTopJetMaterial);
		this.Register(this.originalBottomJetMaterial);
	}

	function Register(originalMaterial : Material) { 
		var meshRendererIndex : uint = 0;
		for(var component in this.parent.model.transform) { 
			var transform : Transform = component as Transform;
			var meshRenderer : MeshRenderer = transform.GetComponent(MeshRenderer);
			if(meshRenderer != null) {
				if(meshRenderer.sharedMaterials.Length > 0) {
					var materialIndex : uint = 0;
					for(var mat : Material in meshRenderer.sharedMaterials) { 
						if(mat == originalMaterial) { 
							this.recolorTable[originalMaterial] = Indices(meshRendererIndex,materialIndex);
							return true;
						}
						materialIndex++;
					}
				}
			}
			meshRendererIndex++;
		}
		return false;
	}
	
	function Recolor(originalMaterial : Material, color : Color) { 
		var data : Indices = this.recolorTable[originalMaterial] as Indices;
		if(data != null) { 
			var transform : Transform = this.parent.model.transform.GetChild(data.meshRendererIndex) as Transform;
			var meshRenderer : MeshRenderer = transform.GetComponent(MeshRenderer);
			meshRenderer.materials[data.materialIndex].color = color;
		}
	}
	
	function UpdateJets(state) { 
		this.state = state;
		if(state == 0) {
			this.Recolor(this.originalTopJetMaterial, this.originalTopJetMaterial.color);
			this.Recolor(this.originalBottomJetMaterial, this.originalBottomJetMaterial.color);
		} else if(state == 1) {
			this.Recolor(this.originalTopJetMaterial, Color.green); 
			this.Recolor(this.originalBottomJetMaterial, Color.green); 
		} else if(state == 2) {
			this.Recolor(this.originalTopJetMaterial, Color.red);
			this.Recolor(this.originalBottomJetMaterial, Color.red); 
		}
	}
	
	function DrawLine(initialPosition:Vector3, finalPosition:Vector3) { 

		//Turn on Gizmos to draw line
		Debug.DrawLine(initialPosition, finalPosition, Color.red);
	}
};

class World {
	
	var stopped : boolean;
	var instantiateFunc : Function;
	var topJetMaterial : Material;
	var bottomJetMaterial : Material;
	var objects : Array;
	var entity : Entity;
	var controls : Controls;
	var scrollPosition : Vector2;
	var scrollHeight : uint;
	var elementHeight : uint; 
	var sessionTimer : Timer;
	var pathTimer : Timer;

	function World(instantiateFunc : Function, topJetMaterial : Material, bottomJetMaterial : Material) { 
		this.stopped = false;
		this.instantiateFunc = instantiateFunc;
		this.topJetMaterial = topJetMaterial;
		this.bottomJetMaterial = bottomJetMaterial;
		this.objects = new Array();
		this.entity = null;
		this.controls = new Controls(this);
		this.scrollPosition = Vector2.zero;
		this.scrollHeight = 0;
		this.elementHeight = 20;
		this.sessionTimer = new Timer(Config.SendTime());
		this.pathTimer = new Timer(Config.PathTime());
		try {
			Program.Client.StartReceiving();
		} catch (e : NullReferenceException) {
			this.stopped = true; //stop further updates to the world
			Application.LoadLevel("LoginScene"); //return to login if not connected
		}
	}
	
	function UpdateSession() {
		var player : Player = Program.Client.player;
		var user : User = Program.Client.player.user;	
		var send : Send = Program.Client.send;
	
		user.checkObjectsMutex.WaitOne();
		if(user.checkObjectsState == 1) {
			user.checkObjectsRadius = Camera.main.farClipPlane;
			user.objectsQueue.clear(); //the entities in the world change 
			for(var o in this.objects) {
				var obj : Entity = o as Entity;
				user.objectsQueue.push(obj.sessionid);
			}
			send.Enqueue(player, send.ObjectsData);
			user.checkObjectsState = 0; //turn off
		}
		user.checkObjectsMutex.ReleaseMutex();	
		Program.Client.SendPackets();
	}
	
	function UpdatePath() {
		var player : Player = Program.Client.player;
		var user : User = Program.Client.player.user;	
		var send : Send = Program.Client.send;
		
		var transformStruct : TransformStruct = new TransformStruct();	
		if(this.controls.states["forward"] == 1) {
			transformStruct.action = 1;
		} else {
			transformStruct.action = 0;
		}	
		transformStruct.position = this.entity.model.position;
		transformStruct.rotation = this.entity.model.rotation;
		user.transformPlayerStruct.sessionid = this.entity.sessionid;
		user.transformPlayerStruct.path.push(transformStruct); 
		
		if(user.transformPlayerStruct.path.length*this.pathTimer.limit >= Config.SendTime()) {
			//Send the data after 5 nodes have been added to the path
			send.Enqueue(player, send.TransformData); //Now, send the transform data
			user.transformPlayerStruct.path.clear();
		}
	}
	
	function UpdateTimers() {
		if(this.entity != null) {
			this.pathTimer.Update(this.UpdatePath as Function);
			this.sessionTimer.Update(this.UpdateSession as Function);
		}
	}
	
	function UpdateRemoteControls() {
		var user : User = Program.Client.player.user;
		//Update previously registered remote controls
		this.ForEntities((function(ent : Entity) { 
			if(ent instanceof RemoteControlledEntity) { 
				(ent as RemoteControlledEntity).remoteControls.Move(); 
			}
		}) as Function);
	
		//Add received remote controls
		user.transformEntityMutex.WaitOne();
		while(user.transformEntityQueue.length > 0) {
			var transformEntityStruct : TransformEntityStruct = user.transformEntityQueue.pop() as TransformEntityStruct;
			for(var o in this.objects) {
				var obj : Entity = o as Entity;
				if(obj.sessionid == transformEntityStruct.sessionid) {
					if(obj instanceof RemoteControlledEntity) {
						(obj as RemoteControlledEntity).remoteControls.Clear();
						while(transformEntityStruct.path.length > 0) {
							var transformStruct : TransformStruct = transformEntityStruct.path.pop() as TransformStruct;
							(obj as RemoteControlledEntity).remoteControls.AddTarget(transformStruct.action, transformStruct.position, transformStruct.rotation); //move before next update
						}
					}
				}
			}
		}
		user.transformEntityMutex.ReleaseMutex();
	}
	
	function UpdateCreateEntities() {
		var user : User = Program.Client.player.user;
	
		user.createEntityMutex.WaitOne();
		while(user.createEntityQueue.length > 0) {
			var createEntityStruct : CreateEntityStruct = user.createEntityQueue.pop() as CreateEntityStruct;
			var instantiatedModel  = this.instantiateFunc();
			var entity : Entity = new Entity(instantiatedModel, this.topJetMaterial, this.bottomJetMaterial);
			
			if(createEntityStruct.mode == 2) { //Handle creating remote entities before setting the entity's parameters
				entity.mode = createEntityStruct.mode;
				if(this.entity != null) {
					entity = new RemoteControlledEntity(instantiatedModel, this.topJetMaterial, this.bottomJetMaterial);
					entity.mode = 0;
				}
			} else {
				entity = new RemoteControlledEntity(instantiatedModel, this.topJetMaterial, this.bottomJetMaterial);
				entity.mode = createEntityStruct.mode;
			}
			entity.model.name = createEntityStruct.displayName;
			entity.sessionid = createEntityStruct.sessionid;
			entity.model.transform.position = createEntityStruct.position;
			entity.model.transform.rotation = createEntityStruct.rotation;
			
			if(entity.mode == 2) { //Handle the case where the entity is the player
				if(this.entity == null) {
					this.entity = entity;
					this.controls.camera.Apply(this.entity.model, this.entity.view);
				}
			}
			
			this.objects.push(entity); //add the entity to the game world
		}
		user.createEntityMutex.ReleaseMutex();
	}
	
	function UpdateDestroyEntities() {
		var user : User = Program.Client.player.user;
	
		user.destroyEntityMutex.WaitOne();
		while(user.destroyEntityQueue.length > 0) {
			var destroyEntityStruct : DestroyEntityStruct = user.destroyEntityQueue.pop() as DestroyEntityStruct;
			
			for(var i : uint = 0; i < this.objects.length; i++) {
				if((this.objects[i] as Entity).sessionid == destroyEntityStruct.sessionid) {
					if(destroyEntityStruct.sessionid != this.entity.sessionid) { 
						UnityEngine.Object.Destroy((this.objects[i] as Entity).model.gameObject);
						this.objects.RemoveAt(i);
					}
				}
			}
		}
		user.destroyEntityMutex.ReleaseMutex();
	}
	
	function Update() {
		if(this.stopped) {
			return;
		}
		this.UpdateDestroyEntities();
		this.UpdateCreateEntities();
		this.UpdateRemoteControls();
		this.UpdateTimers();
		this.controls.Update();
	}

	function ForEntities(func : Function) { 
		for(var obj in this.objects) { 
			var entity : Entity = obj as Entity;
			func(entity);
		}
	}
	
	function OnControlsFocus(state : boolean) {
		this.controls.focussed = state;
	}
	
	function OnGUI() {
		if(this.entity != null) {
			this.ForEntities((function(entity : Entity) {
				if(entity.IsVisible()) {
					entity.gui.screenPosition = Camera.main.WorldToScreenPoint(entity.model.transform.position);
					if(entity.gui.OnTag()) { 
						entity.controls.ToggleOpen();
					}
					if(entity.controls.IsOpen()) {
						entity.gui.OnContents();
					}
				}
			}) as Function);
		}else{
			GUI.Box(Rect(0,0,Screen.width,Screen.height), "Loading...");
		}
	}
	
	function OnDestroy() {
		try {
			Program.Client.StopReceiving();
			Program.Client.tcpClient.Close();
		} catch(e : NullReferenceException) {
		
		}
	}
	
};


class RemoteControlledEntity extends Entity {

	var remoteControls : RemoteController;
	
	class Target extends TransformStruct {
		var initialized : boolean;
		var canTranslate : boolean; 
		var translateSpeed : float;
		var rotateSpeed : float;
		
		function Target() {
			super();
			this.initialized = false;
			this.canTranslate = false;
			this.translateSpeed = 0.0;
			this.rotateSpeed = 0.0;
		}
		
	};
	
	class RemoteController {
		var parent : RemoteControlledEntity;
		var target : Target; //current target
		var targets : Array; //move to controls
		var lean : Lean;
		var nodeTimer : Timer;
	
		function RemoteController(parent : RemoteControlledEntity) {
			this.parent = parent;
			this.target = new Target();
			this.targets = new Array();
			this.lean = new Lean();
			this.nodeTimer = new Timer(Config.PathTime()); 
		}
		
		function Clear() {
			if(this.target.initialized) {
				this.parent.model.position = this.target.position;
				this.parent.model.rotation = this.target.rotation;
			}
			this.targets.clear();
		}
		
		function AddTarget(action : uint, position : Vector3, rotation : Quaternion) {
			var addedTarget : Target = new Target();
			addedTarget.action = action;
			addedTarget.position = position;
			addedTarget.rotation = rotation;
			addedTarget.initialized = true;
			this.targets.push(addedTarget);
		}
		
		function Next() {
			if(this.targets.length > 0) {
				var nextTarget : Target = this.targets.pop() as Target;
				if(!this.target.initialized) {
					this.lean.Begin(this.parent.model);
				}
				this.target = nextTarget;
				this.target.translateSpeed = Vector3.Distance(this.parent.model.position, this.target.position) / this.nodeTimer.limit; 
				//Note: Arc-Distance with radius 1 is equal to the central angle (in terms of quaternion angle)
				//The angle between two quaternions can be calculated with: theta = arccos(2(a1*a2+b1*b2+c1*c2+d1*d2)-1), but we can use unity's built-in functions.
				this.target.rotateSpeed = Quaternion.Angle(this.parent.model.rotation, this.target.rotation) / this.nodeTimer.limit;
			}
			
		}
	
		function Move() {	
		
		
			for(var i : uint = 0; i < this.targets.length; i++){ 
				if(i != 0) {
					Debug.DrawLine((targets[i-1] as Target).position, (targets[i] as Target).position, Color.cyan);
				}
			}
			
			if(this.nodeTimer.IsDone()) {
			
				if(this.target.initialized) {
					this.parent.model.position = this.target.position;
					this.parent.model.rotation = this.target.rotation;
				}
				
				if(this.target.action == 0) {
					if(!this.lean.Ended()) {
						this.lean.Back(this.parent.model);
						if(!this.parent.view.state != 2) {
							this.parent.view.UpdateJets(2);
						}
					} else {
						this.parent.view.UpdateJets(0);
						this.lean.Reset();
						this.lean.Begin(this.parent.model);
						this.Next();
						this.nodeTimer.Reset();
					}
				} else {
					this.Next();
					this.nodeTimer.Reset();
				}
				
					
			} else {
				this.nodeTimer.Down();
			}
		
			if(this.target.initialized) {
				if(this.target.rotateSpeed != 0) {
					this.parent.model.rotation = Quaternion.RotateTowards(this.parent.model.rotation, this.target.rotation, this.target.rotateSpeed*Time.deltaTime); //clamped - will not rotate further than required
				}
				var temp : Vector3 = this.parent.model.position;
				this.parent.model.position = Vector3.MoveTowards(this.parent.model.position, this.target.position, this.target.translateSpeed*Time.deltaTime); //clamped - will not move further along line
				if(!Mathf.Approximately(temp.sqrMagnitude,this.parent.model.position.sqrMagnitude)) {
					if(this.target.action == 1) {
						this.lean.Forward(this.parent.model);
					}
					if(!this.parent.view.state != 1) {
						this.parent.view.UpdateJets(1);
					}
				}
			} 
		}
		
	};
	
	function RemoteControlledEntity(model : Transform, topJetMaterial : Material, bottomJetMaterial : Material) {
		super(model, topJetMaterial, bottomJetMaterial);
		this.remoteControls = new RemoteController(this);
		UnityEngine.Object.Destroy(model.gameObject.GetComponent(Rigidbody)); //motion is controlled remotely; do not let the physics engine dictate its motion
		UnityEngine.Object.Destroy(model.gameObject.GetComponent(BoxCollider)); 
	}
	
};

class Entity {
	var model : Transform;
	var view : View;
	var sessionid : uint;
	var mode : uint;
	var gui : EntityGUI;
	var parent : Entity;
	var controls : OpenerController;
	var container : Container;

	class EntityGUI { 
		var parent : Entity;
		var currentScript : String;
		var screenPosition : Vector3;
		var elementHeight : uint;
		var containerScrollPosition : Vector2;
		var containerScrollHeight : uint;
		var scriptScrollPosition : Vector2;
		function EntityGUI(parent : Entity) { 
			this.parent = parent;
			this.screenPosition = Vector3.zero;
			this.elementHeight = 20;
			this.scriptScrollPosition = Vector2.zero;
			this.containerScrollPosition = Vector2.zero;
			this.containerScrollHeight = 0;
			this.currentScript = "";
		}
		//Make part of "models" or "gui"
		function OnTag() { 
			var ret : boolean = false;
			switch(this.parent.mode) {
				case 0: //Only show Tag
					var rect : Rect = Rect(this.screenPosition.x-(GUI.skin.label.CalcSize(new GUIContent(this.parent.model.name)).x/2), Screen.height-this.screenPosition.y, 100, 30);
					GUI.Label(rect, this.parent.model.name);
				break;
				case 1: //Clickable
					ret = GUI.Button(Rect(this.screenPosition.x-100/2, Screen.height-this.screenPosition.y, 100, 30), this.parent.model.name);
				break;
				case 2: //Don't show anything - controlled by camera
				break;
			
			}
			return ret;
		}
		
		function OnScroller(title : String, innerFunc : Function) {
			GUI.Box(Rect(this.screenPosition.x-300/2, Screen.height-(this.screenPosition.y+300+this.elementHeight), 300, 300), title);
			this.containerScrollPosition = GUI.BeginScrollView(Rect(this.screenPosition.x-300/2, Screen.height-(this.screenPosition.y+300), 318, 300), this.containerScrollPosition, Rect(0,0,300,this.containerScrollHeight));
			innerFunc();
			GUI.EndScrollView();
		}
		
		function OnContents() {
			this.OnScroller("Bin", (function() { 
				var yOffset : uint = 0;
				var y : uint = 0;
				this.parent.container.ForEach((function(i : uint, item : Entity.Container.Item) { 
					y = i*this.elementHeight + yOffset;
					if(GUI.Button(Rect(0,y,300,this.elementHeight), item.Name())) {
							item.controls.ToggleOpen();
					}
					if(item.controls.IsOpen()) {
							GUI.BeginGroup(Rect(0, y+this.elementHeight, 300, 100)); //group size 120
							var idx : uint = 0;
							var labelFunc : Function = (function(label : String) { 
								GUI.Box(Rect(0, idx*this.elementHeight, 300, this.elementHeight), label);
								idx++;
							}) as Function;
							var buttonFunc : Function = (function(innerItem : Entity.Container.Item) { 
								if(GUI.Button(Rect(0, idx*this.elementHeight, 300, this.elementHeight), innerItem.Name())) {
									innerItem.controls.ToggleOpen();
								}
								idx++;
							}) as Function;
							item.ForEach(labelFunc, buttonFunc);
							GUI.EndGroup();
							yOffset += 100+this.elementHeight; //group size 120
						}
				}) as Function);
				this.containerScrollHeight = y; //height is largest y value
			}) as Function);
		}
	
	};
	
	
	class OpenerController {
	
		var states : Hashtable;
		function OpenerController() {
			this.states = new Hashtable();
			this.states["open"] = 0; //state controlling the item container
		}
	
		function ToggleOpen() { 
			if(this.states["open"] == 0) { 
				this.states["open"] = 1;
			} else {
				this.states["open"] = 0;
			}
		}
		
		function IsOpen() { 
			return this.states["open"] == 1;
		}
		
	};
	
	class Container { 
		var parent : Entity;
		var items : Array; //store data to prevent useless computations each frame
		
		class Item {

			var controls : OpenerController;
			var parent : Container;
			var properties : Hashtable;
			
			function Item(parent : Container) {
				this.parent = parent;
				this.properties = new Hashtable();
				this.controls = new OpenerController();
			}
			
			function Name() { 
				if(this.properties.ContainsKey("name")) { 
					if(this.properties["name"] instanceof String) {
						return (this.properties["name"] as String);
					}
				}
				return "Undefined";
			}
			
			function ForEach(labelFunc : Function, buttonFunc : Function) { 
				for(var k in this.properties.Keys) { 
					var label : String = "";
					if(k instanceof String) {
						var key : String = k as String;
						label += key; 
						if(this.properties[key] instanceof String) { 
							label += ":" + (this.properties[key] as String);
							labelFunc(label);
						} else if(this.properties[key] instanceof Item) {
							var val = this.properties[key] as Item;
							buttonFunc(val);
							if(val.controls.IsOpen()) {
								 val.ForEach(labelFunc, buttonFunc); 
							}
							//This is recursive; TODO: render gui here instead of compiling strings
						}
					}
				}
			}
				
		};
		
		function Container(parent : Entity) { 
			this.parent = parent;		
			this.items = new Array();
			this.Build(["Helmet", "Jacket", "Pants", "Gloves", "Boots"]);
		}
		
		function Build(names : Array) { 
			for(var i = 0; i < names.length; i++) {
				var item = new Item(this);
				item.properties["name"] = names[i];
				var test : Item = new Item(this);
				test.properties["name"] = "Test";
				item.properties["test"] = test;
				this.items.push(item);
			}
		}
		
		function ForEach(func : Function) { 
			for(var i : uint = 0; i < this.items.length; i++) { 
				func(i, (items[i] as Item));
			}
		}
	};
	
	function Entity(model : Transform, topJetMaterial : Material, bottomJetMaterial : Material) { 
		this.model = model;
		this.view = new View(this, topJetMaterial, bottomJetMaterial);
		this.sessionid = 0;
		this.mode = 0;
		this.gui = new EntityGUI(this);
		this.container = new Container(this);
		this.controls = new OpenerController();
	}
	
	
	/** Description: Returns true if any mesh components are visible. **/
	function IsVisible() { 
		for(var component in this.model.transform) { 
			var transform : Transform = component as Transform;
			var meshRenderer : MeshRenderer = transform.GetComponent(MeshRenderer);
			if(meshRenderer != null) {
				if(meshRenderer.isVisible) {
					return true;
				}
			}
		}
		return false;
	}
	
	
	
};

function Start () {
	var spawnFunc : Function = (function() { 
			var entityInstance : Transform = Instantiate(entityPrefab); 
			var emptyInstance : Transform = Instantiate(emptyPrefab);
			emptyInstance.transform.parent = entityInstance;
			emptyInstance.transform.localRotation.y = 180.0;
			emptyInstance.name = "peripherals";
			var headlightInstance : Transform = Instantiate(headlightPrefab);
			emptyInstance.transform.localRotation.y = 180.0;
			headlightInstance.transform.parent = emptyInstance;
			headlightInstance.transform.localPosition.x = 0.0;
			headlightInstance.transform.localPosition.y = 0.035;
			headlightInstance.transform.localPosition.z = 0.003;
			headlightInstance.name = "light";
			var light : Light = headlightInstance.GetComponent("Light") as Light;
			if(light != null) {
				light.enabled = false; //turn off light by default
			}
			return entityInstance;
			
		}) as Function;
	world = new World(spawnFunc, topJetMaterial, bottomJetMaterial);
}

function Update () {
	world.Update();
}

function OnGUI() {
	world.OnGUI();
}


function OnControlsFocus(state : boolean) {
	world.OnControlsFocus(state);
}



function OnDestroy() {
	world.OnDestroy();
}







