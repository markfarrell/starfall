/** 
 * Filename: Context.js
 * Description: The entry point for a V8 context owned by one entity on the server.
 * Date: July 25th, 2013 
 */ 

function Main() { 
    var entity = entity || {}; //reference the entity if it was already passed by the server or create it.
    entity.inventory = new Inventory(); //extend the entity object to include its inventory
    //TODO: create item and apparel classes; apparel will be able to update entity.appearance, which will also be handled in javascript
}