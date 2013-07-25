/** 
 * Place 'Network.js' in standard assets.
 * Unity compiles these assets first.
 * So if network is to be imported in scripts that are attached to
 * game objects at runtime, it is necessary to include it in this folder. 
 */

import System.Net.Sockets;
import System.Text;
import System.Threading;


/*
 * Date: April 21st, 2013
 * Description: Contains all of the data relevant to user interaction upon connecting.
 * It can be passed to the client's init function as reference for modification.
 */
class GUIStruct {
	var connectionStatus : boolean;
	var message : String;
	var userFieldString : String;
	var userLabelString : String;
	var passwordFieldString : String;
	var passwordLabelString : String;
	
	function GUIStruct() {
		this.connectionStatus = false;
		this.userFieldString = "";
		this.userLabelString = "Username:";
		this.passwordFieldString = "";
		this.passwordLabelString = "Password:";
		this.message = "";
	}
}

class LoginStruct {

	var state : int; //0->Not Logged In, 1->Logging In, 2->Logged In, 3->Logging Out
	var userid : uint;
	var usertype : uint;
	var username : String;
	var password : String;
	
	function LoginStruct() {
		this.state = 0;
		this.userid = 0;
		this.usertype = 0;
		this.username = "";
		this.password = "";
	}
}

class TransformStruct {
	var action : uint;
	var position : Vector3;
	var rotation : Quaternion;
	function TransformStruct() {
		this.action = 0;
		this.position = Vector3.zero;
		this.rotation = Quaternion.identity;
	}
};

class TransformEntityStruct {
	var sessionid : uint;
	var path : Array;
	function TransformEntityStruct() {
		this.sessionid = 0;
		this.path = new Array();
	}
};

class CreateEntityStruct {

	var displayName : String;
	var appearance : String;
	var sessionid : uint;
	var mode : uint;
	var position : Vector3;
	var rotation : Quaternion;

	function CreateEntityStruct() {
		this.displayName = "";
		this.appearance = "";
		this.sessionid = 0;
		this.mode = 0;
		this.position = Vector3.zero;
		this.rotation = Quaternion.identity;
	}
};

class DestroyEntityStruct {

	var sessionid : uint;
	
	function DestroyEntityStruct() {
		this.sessionid = 0;
	}
};


class User {
	var address : String;
	var port : int;
	var checkObjectsState : uint; //0->off, 1->on
	var checkObjectsRadius : float; //far clip plane
	var checkObjectsMutex : Mutex;
	var sendQueue : Array;
	var loadLevelQueue : Array;
	var consoleQueue : Array; //Send data directly to the web console
	var objectsQueue : Array; //Stores registered session IDs
	var transformPlayerStruct : TransformEntityStruct;
	var createEntityQueue : Array; //The World class polls the create entity queue
	var createEntityMutex : Mutex;
	var transformEntityQueue : Array; //The World class polls the transform entity queue
	var transformEntityMutex : Mutex;
	var destroyEntityQueue : Array; //The World class polls the destroy entity queue
	var destroyEntityMutex : Mutex;
	
	function User(address:String,port:int){
		this.address = address;
		this.port = port;
		this.checkObjectsRadius = 0.0;
		this.checkObjectsState = 1; //on at first
		this.checkObjectsMutex = new Mutex();
		this.sendQueue = new Array();
		this.loadLevelQueue = new Array(); //It should only be possible for the user to ask to load one level at a time.
		this.consoleQueue = new Array();
		this.objectsQueue = new Array();
		this.transformPlayerStruct = new TransformEntityStruct(); //user path, to be sent to the server.
		this.createEntityQueue = new Array();
		this.createEntityMutex = new Mutex(); //current thread does not own the mutex
		this.transformEntityQueue = new Array();
		this.transformEntityMutex = new Mutex(); //current thread does not own the mutex
		this.destroyEntityQueue = new Array();
		this.destroyEntityMutex = new Mutex(); //current thread does not own the mutex
	}
	

}

/* Note: The player class follows a different inheritance model than the server version.
 * This is due to a language limitation. */
class Player {
	var user:User;
	var login:LoginStruct;
	
	function Player(address:String,port:int) {
		this.user = new User(address,port);
		this.login = new LoginStruct();
	}
	
	function IsLoggedIn() {
		return (this.login.state == 2);
	}
	
	function IsLoggingOut() { 
		return (this.login.state == 3);
	}
	
	function Logout() {
		this.login.state = 3;
		this.OnState();
	}
	
	function OnState() { 
		switch(this.login.state) { 
			case 1:
				this.user.loadLevelQueue.push("WorldScene");
				this.login.state = 2;
			break;
			case 3: //Logout
				this.user.sendQueue.clear();
				Program.Client.StopReceiving();
				Program.Client.tcpClient.Close();
				this.user.loadLevelQueue.push("LoginScene");
				Program.Client.End();
				this.login.state = 0;
			break;
		}
	}
	
}

/**
 * Date: 13/04/2013
 * Description: A mirror of the head structure from the C++ server.
 */
class Head {

	var begin    : uint;
	var opcode   : uint;
	var bodysize : uint;
	var end      : uint;
	
	function Head () {
	
		this.begin    = 0xFFFFFFFF;
		this.opcode   = 0;
		this.bodysize = 0;
		this.end      = 0xFFFFFFFF;
		
	}
	
	/* Description: The number of bytes that should be in the header. */
	static function Size() {
		return 16;
	}
}

class Buffer {

	var data : Array;
	//Design: Read head and write head
	var wpos : uint = 0;
	var rpos : uint = 0;
	
	function Buffer() {
		this.data = new Array();
	}
	
	function Clear() {
		this.wpos = 0;
		this.rpos = 0;
		this.data.clear();
	}
	
	function Length() {
		return data.length;
	}
	
	/**
	 * Date: 13/04/2013
	 * Description: Convert uint(=unsigned 32-bit integer) to byte(=unsigned 8-bit integer)
	 * 				e.g. 00000011 becomes 11
	 * Note 1: an int cannot be converted to a byte implicitly in UnityScript, 
	 * 		   so parsing must be used.
	 * Note 2: Take precaution working between int and uint.
	 * 		   Though int should work for up to half the maximum value of uint.
	 */
	function WriteUInt8(i : uint) {
		this.wpos = this.data.length-1;
		var b : byte = 0;
			//Isolate the byte with (i&0xFF)
		if(byte.TryParse((i&0xFF).ToString(), b)) {
				this.data.push(b);
				this.wpos++;
		} else {
				Debug.Log("Error converting: " + i + " to a byte! It will not be inserted into the array.");
		}
	}
	
	function WriteUInt32(i : uint) {
		WriteUInt8(i>>24);
		WriteUInt8(i>>16);
		WriteUInt8(i>>8);
		WriteUInt8(i);
	}
	
	function WriteString(s : String) {
		this.wpos = this.data.length-1;
		WriteUInt32(s.Length);

		var bytearray : byte[] = Encoding.ASCII.GetBytes(s);
		// Note: typeof(s[i]) == System.Char. System.Char is encoded as UTF-16. So it is necessary to change the encoding to ASCII. 
		// Warning: Do not use data.concat(...), since we are dealing with builtin arrays (.NET) and not true JavaScript arrays.
		for(var i : int = 0; i < bytearray.length; i++) {
			this.data.push(bytearray[i]);
			this.wpos++;
		}
			
	}
	
	function WriteFloat(f : float) {
		this.WriteString(f.ToString());
	}
	
	function ReadUInt32() {
		if((this.rpos+4-1) < data.length){
		
			var a : byte=0; 
			var b : byte=0; 
			var c : byte=0; 
			var d : byte=0; 
			
			byte.TryParse(this.data[this.rpos].ToString(), a); this.rpos++;
			byte.TryParse(this.data[this.rpos].ToString(), b); this.rpos++;
			byte.TryParse(this.data[this.rpos].ToString(), c); this.rpos++;
			byte.TryParse(this.data[this.rpos].ToString(), d); this.rpos++;
			
			return (a<<24)|(b<<16)|(c<<8)|(d);
		}else{
			//TODO(April 20th, 2013): Add error / catch case
			return 0;
		}
	}
	
	function ReadString() {
		var l : uint = this.ReadUInt32();
		var builder : StringBuilder = new StringBuilder(l);
		for(var i : int = 0; i < l; i++){
			var c : byte[] = new byte[1];
			byte.TryParse(this.data[this.rpos].ToString(), c[0]); this.rpos++;
			builder.Append(Encoding.ASCII.GetChars(c)[0]);
		}
		return builder.ToString();
	}
	
	function ReadFloat() {
		var ret : float = 0.0;
		if(!float.TryParse(this.ReadString(), ret)) {
			throw new System.ArgumentException("Buffer.ReadFloat : Parsing float error.");
		}
		return ret;
	}

}


class Send {

	var map : Hashtable;
	
	/* Returns: a boolean. */
	function Enqueue(player : Player, sendFunction : Function) {
	
		var opcode : uint;
		uint.TryParse(this.map[sendFunction].ToString(), opcode);
		var body : Buffer = sendFunction(player) as Buffer;
		
		if(body != null && opcode != null) {
			var buffer : Buffer = new Buffer();
			
				var head = new Head();
				head.opcode = opcode;
			
			
				buffer.WriteUInt32(head.begin);
				buffer.WriteUInt32(head.opcode);
				/* We will replace the bodysize once it has been updated. So, obtain the index at which it is stored.
				 * Note: Its value is "data.length" and not "data.length-1" because the next byte is always written 
				 * at a position not yet created. */
				var indexBodySize : uint = buffer.Length(); 
				buffer.WriteUInt32(0); //Temporarily store 0 for the bodysize
				buffer.WriteUInt32(head.end);
			
				/* Always fill the rest of the bytes needed in the header */
				var l : int = buffer.Length(); //must get length before loop
				for(var i : int = 0; i < (Head.Size() - l); i++) {
					buffer.WriteUInt8(0);
				}
			
				var begin : uint = buffer.Length(); 	//Begin Writing Body; Store Position
				
				
				for(var j :int = 0; j < body.data.length; j++){
					buffer.data.push(body.data[j]); buffer.wpos++;
				}
				
		
				var end : uint = buffer.Length(); //End Writing Body; Store Position
			
				/* Now, write the body size. Note: This code could be expressed with a loop. 
				 * However, its purpose feels clearer when written explicitly. */
				head.bodysize = end-begin;
				buffer.WriteUInt32(head.bodysize); //Push an integer, then pop the bytes
				var d = buffer.data.pop(); buffer.wpos--; //Pop is in reverse order
				var c = buffer.data.pop(); buffer.wpos--;
				var b = buffer.data.pop(); buffer.wpos--;
				var a = buffer.data.pop(); buffer.wpos--;
				buffer.data[indexBodySize] = a; //Replace the bytes in the head with the correct value of head.bodysize
				buffer.data[indexBodySize+1] = b;
				buffer.data[indexBodySize+2] = c;
				buffer.data[indexBodySize+3] = d;
				player.user.sendQueue.push(buffer);
				return true;
		}
		return false;
	}
	
	
	
	/* Returns: A buffer object representing the body of the packet. */
	function LoginData(player:Player) {
		var buffer :Buffer = new Buffer();
		buffer.WriteUInt32(player.login.state); //state
		buffer.WriteUInt32(player.login.userid); 
		buffer.WriteUInt32(player.login.usertype);
		buffer.WriteString(player.login.username);
		buffer.WriteString(player.login.password);
		return buffer;
	}
	
	function ConsoleData(player:Player) {
		var buffer : Buffer = new Buffer();
		buffer.WriteUInt32(player.user.consoleQueue.length);
		while(player.user.consoleQueue.length != 0) {
			var json : String = player.user.consoleQueue.shift() as String;
			buffer.WriteString(json);
		}
		return buffer;
	}
	
	/** 
	 * Sends the player's transform data, as a path stored in a collection, to the server.
	 */
	function TransformData(player:Player) { 
		var buffer : Buffer = new Buffer();
		buffer.WriteUInt32(player.user.transformPlayerStruct.path.length);
		for(var i in player.user.transformPlayerStruct.path) { 
			var transformStruct : TransformStruct = i as TransformStruct;
			buffer.WriteUInt32(player.user.transformPlayerStruct.sessionid);
			buffer.WriteUInt32(transformStruct.action);
			buffer.WriteString(transformStruct.position.x.ToString());
			buffer.WriteString(transformStruct.position.y.ToString());
			buffer.WriteString(transformStruct.position.z.ToString());
			buffer.WriteString(transformStruct.rotation.x.ToString());
			buffer.WriteString(transformStruct.rotation.y.ToString());
			buffer.WriteString(transformStruct.rotation.z.ToString());
			buffer.WriteString(transformStruct.rotation.w.ToString());
		}
		return buffer;
	}
	
	/** 
	 * Description: Sends a list of session IDs to the server that the client has registered.
	 * If there exists session IDs that this player has not registered, the server will send back
	 * data to create those entities.
	 */
	function ObjectsData(player:Player) {
		var buffer : Buffer = new Buffer();
		buffer.WriteUInt32(player.user.checkObjectsState);
		buffer.WriteFloat(player.user.checkObjectsRadius);
		buffer.WriteUInt32(player.user.objectsQueue.length); //length of IDs to send
		while(player.user.objectsQueue.length != 0) {
			var sessionid : uint = player.user.objectsQueue.shift();
			buffer.WriteUInt32(sessionid);
		}
		return buffer;
	}
	
	function Send() {
		this.map = new Hashtable();
		this.map[(this.LoginData as Function)]= 0x01;
		this.map[(this.ConsoleData as Function)] = 0x02;
		// TODO: 0x03->Install Player
		this.map[(this.TransformData as Function)] = 0x04; //TODO: 
		// TODO: 0x05->Logout/Destroy Player 
		this.map[(this.ObjectsData as Function)] = 0x06;
	}
	

}

class Receive {

	var map : Hashtable;
	
	function LoginReply(player : Player, buffer : Buffer) {
		var loginStruct : LoginStruct = new LoginStruct(); 
		loginStruct.state = buffer.ReadUInt32(); 
		loginStruct.userid  = buffer.ReadUInt32();
		loginStruct.usertype = buffer.ReadUInt32();
		loginStruct.username = buffer.ReadString();
		loginStruct.password = buffer.ReadString();
		Debug.Log("LoginStruct[userid="+loginStruct.userid.ToString("x16")+",username="+loginStruct.username+",password="+loginStruct.password+"]");
		if(player.login.userid != loginStruct.userid) { //The player has been loaded.
			//Now, update the player once the reply has been receive.
			player.login.state = loginStruct.state;
			player.login.userid = loginStruct.userid;
			player.login.usertype = loginStruct.usertype;
			player.login.username = loginStruct.username; //TODO assert equals
			player.login.password = loginStruct.password;
			return true;
		}
		return false;
	}
	
	function CreateEntity(player : Player, buffer : Buffer) { 
		var length : uint = buffer.ReadUInt32();
		for(var i : uint = 0; i < length; i++) {
			var createEntityStruct : CreateEntityStruct = new CreateEntityStruct();
			createEntityStruct.sessionid = buffer.ReadUInt32();
			createEntityStruct.mode = buffer.ReadUInt32(); 
			createEntityStruct.displayName = buffer.ReadString();
			createEntityStruct.position.x = buffer.ReadFloat();
			createEntityStruct.position.y = buffer.ReadFloat();
			createEntityStruct.position.z = buffer.ReadFloat();
			createEntityStruct.rotation.x = buffer.ReadFloat();
			createEntityStruct.rotation.y = buffer.ReadFloat();
			createEntityStruct.rotation.z = buffer.ReadFloat();
			createEntityStruct.rotation.w = buffer.ReadFloat();
			//createEntityStruct.appearance = buffer.ReadString();
			//Debug.Log(createEntityStruct.appearance.Length);
			player.user.createEntityMutex.WaitOne(); //enter if it is safe to enter; claim ownership
			player.user.createEntityQueue.Add(createEntityStruct);
			player.user.createEntityMutex.ReleaseMutex(); //release ownership; it is safe to access the player's entity queue now
		}
	}
	
	function TransformEntity(player : Player, buffer : Buffer) { 
		var entityLength : uint = buffer.ReadUInt32();
		for(var i : uint = 0; i < entityLength; i++) {
			var transformEntityStruct : TransformEntityStruct = new TransformEntityStruct();
			transformEntityStruct.sessionid = buffer.ReadUInt32();
			var pathLength : uint = buffer.ReadUInt32();
			for(var j : uint = 0; j < pathLength; j++) {
				var transformStruct : TransformStruct = new TransformStruct();
				transformStruct.action = buffer.ReadUInt32();
				transformStruct.position.x = buffer.ReadFloat();
				transformStruct.position.y = buffer.ReadFloat();
				transformStruct.position.z = buffer.ReadFloat();
				transformStruct.rotation.x = buffer.ReadFloat();
				transformStruct.rotation.y = buffer.ReadFloat();
				transformStruct.rotation.z = buffer.ReadFloat();
				transformStruct.rotation.w = buffer.ReadFloat();
				transformEntityStruct.path.push(transformStruct);
			}
			player.user.transformEntityMutex.WaitOne(); //enter if it is safe to enter; claim ownership
			player.user.transformEntityQueue.Add(transformEntityStruct);
			player.user.transformEntityMutex.ReleaseMutex(); //release ownership; it is safe to access the player's entity queue now
		}
	}
	
	function DestroyEntity(player : Player, buffer : Buffer) { 
		var length : uint = buffer.ReadUInt32();
		for(var i : uint = 0; i < length; i++) {
			var destroyEntityStruct : DestroyEntityStruct = new DestroyEntityStruct();
			destroyEntityStruct.sessionid = buffer.ReadUInt32();
			player.user.destroyEntityMutex.WaitOne(); //enter if it is safe to enter; claim ownership
			player.user.destroyEntityQueue.Add(destroyEntityStruct);
			player.user.destroyEntityMutex.ReleaseMutex(); //release ownership; it is safe to access the player's entity queue now
		}
	}
	
	function ObjectsReply(player : Player, buffer : Buffer) { 
		player.user.checkObjectsMutex.WaitOne();
		player.user.checkObjectsState = buffer.ReadUInt32();
		player.user.checkObjectsMutex.ReleaseMutex();
		this.CreateEntity(player, buffer);
		this.DestroyEntity(player, buffer); 
	}
	
	
	function Receive() {
		this.map = new Hashtable();
		//Note: uints are not object types, so they are not hashable. They have to be converted to String objects.
		this.map[((0x10000000 | 0x01).ToString())] = this.LoginReply;
		//no 0x02 opcode equivalent
		this.map[((0x10000000 | 0x03).ToString())] = this.CreateEntity;
		this.map[((0x10000000 | 0x04).ToString())] = this.TransformEntity;
		this.map[((0x10000000 | 0x05).ToString())] = this.DestroyEntity;
		this.map[((0x10000000 | 0x06).ToString())] = this.ObjectsReply;
	}
}

class GameClient {

	var isReceiving : boolean;
	var tcpClient : TcpClient;
	var receiveThread : Thread;
	var player : Player;
	var send : Send;
	var receive : Receive;
	
	function GameClient(address:String,port:int) {
		this.isReceiving = false;
		this.tcpClient = new TcpClient();
		this.receiveThread = new Thread(new ThreadStart((function() { //Keep the run function private
			 while(this.isReceiving) { 
			 	this.ReceivePacket(); //blocking sockets
			 }
			 Debug.Log("Not receiving");
		})));
		this.send = new Send();
		this.receive = new Receive();
		this.player = new Player(address,port);
	}
	
	function StartReceiving() { 
		this.isReceiving = true;
		this.receiveThread.Start();
	}
	
	function StopReceiving() {
		this.isReceiving = false;
		this.receiveThread.Abort(); //the thread can be interrupted instead; (aborted threads cannot be restarted); TODO: use a finally block or catch exceptions in the thread
		//this.receiveThread.Join(); //blocks this thread until receiveThread has stopped; might block until socket is timed out!
	}
	
	function ReceivePacket() { 
		//TODO(April 19th,2013): Place the reading in a separate thread.
		var receivedHeaderBytes : byte[] = new byte[Head.Size()];
		var n : int = this.tcpClient.Client.Receive(receivedHeaderBytes);			
		if(n==0) {
			return;
		}
			
		var buffer : Buffer = new Buffer();
		for(var i : int = 0; i < n; i++){
			buffer.data.push(receivedHeaderBytes[i]);
		}
			
		var head : Head = new Head();
		head.begin = buffer.ReadUInt32();
		head.bodysize = buffer.ReadUInt32();
		head.opcode = buffer.ReadUInt32();
		head.end = buffer.ReadUInt32();
					
		var receivedBodyBytes : byte[] = new byte[head.bodysize];
		var n2 : int = this.tcpClient.Client.Receive(receivedBodyBytes);
		buffer.Clear();
		for(var j : int = 0; j < n2; j++){
			buffer.data.push(receivedBodyBytes[j]);
		}
				
		var receiveFunction : Function = this.receive.map[head.opcode.ToString()] as Function;
		if(receiveFunction != null) {
			receiveFunction(this.player, buffer);
		} else {
			Debug.Log("Warning: Receive function is null! Opcode: "+head.opcode.ToString("x16"));
		}
		
	}
	
	function SendPackets() {
		try { 
			while(this.player.user.sendQueue.length != 0) {
				var buffer : Buffer = this.player.user.sendQueue.shift() as Buffer;
				//Convert the data to a .NET type (a byte array) and send it
				this.tcpClient.Client.Send((buffer.data.ToBuiltin(byte) as byte[]));
			}
		}catch(socketException : SocketException) {
			this.player.Logout();
		}
	}
	
	/* Returns: An updated GUIStruct.*/
	function Init(gui:GUIStruct) {
	
		if(gui.userFieldString.Length == 0 || gui.passwordFieldString.Length == 0) {
			gui.message = "Enter username/password!";
			gui.connectionStatus = false;
			return gui;
		}
	
		try {
	
			this.tcpClient.Connect(this.player.user.address, this.player.user.port);

			this.player.login.username = gui.userFieldString;
			this.player.login.password = gui.passwordFieldString;
		
			if(this.tcpClient.Connected) {
				this.send.Enqueue(player, this.send.LoginData);
				this.SendPackets();
				this.ReceivePacket();
			}


		
			gui.message = "Logging in...";
			gui.connectionStatus = true;
			
			player.OnState(); //finish logging in, or do nothing.
			if(!player.IsLoggedIn()) { 
				gui.message = "Failed to login.";
			}
			
			return gui;
	
		
		} catch(socketException : SocketException) {
			Debug.Log(socketException.Message);
		}
	
		gui.message = "Failed to connect.";
		gui.connectionStatus = false;
	
		return gui;
	}
	
	/* Description: End() is called when the server and client are done talking in this scene.
	 * Here, the next level will be loaded. */
	function End() {
		if(this.player.user.loadLevelQueue.length > 0){
			var nextLevel : String = this.player.user.loadLevelQueue.shift() as String;
						//Check: http://docs.unity3d.com/Documentation/ScriptReference/EditorApplication.OpenScene.htm
			this.player.user.loadLevelQueue.clear();
			Application.LoadLevel(nextLevel);
			return true;
		}
		return false;
	}
}

/** 
 * The Program class contains static objects that can be shared between scenes in Unity.
 */
class Program {
	static var Client : GameClient; //Client is initialized in the LoginScene; Then it is passed to the next scene.
}