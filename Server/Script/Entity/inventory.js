var Inventory = Inventory || function() {
    
    
};

Inventory.default = function() {
        var defaultInventory = JSON.parse(System.Data("Entity/inventory.json"));
        if(_.isArray(defaultInventory)) {
            return defaultInventory;
        }
        return [];   
    }
    this.default = default;  
};
