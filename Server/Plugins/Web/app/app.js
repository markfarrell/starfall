//Copyright (c) 2013 Mark Farrell

function App () {

        this.filters.registerClick();
        this.modifiers.appearance.registerClick();
        this.components.filterFunc = this.filters.filter.bind(this.filters);
        this.console.hideFunc = this.filters.hide.bind(this.filters);
        this.news.hideFunc = this.filters.hide.bind(this.filters); //'this' in the context of news can't access render, so we can reference it inside of news.
        this.more.hideFunc = this.filters.hide.bind(this.filters);
        this.console.renderFunc = this.components.render.bind(this.components);
        this.news.renderFunc = this.components.render.bind(this.components); //'this' in the context of news can't access render, so we can reference it inside of news.
        this.more.renderFunc = this.components.render.bind(this.components);
        this.news.dataFunc = (function() { return { upper: this.updater.db.max, offset : 0 }; }).bind(this);
        this.login.registerClick({ next: (function () { this.console.ajax().done(this.init.bind(this)); }).bind(this) }); // "Initialize" the app after the user has logged in.

    } //class definition

    App.prototype = {

        storeMax : function(currentDatabaseMax) {
             if(currentDatabaseMax > this.updater.db.max) {
                        this.updater.db.max = currentDatabaseMax; //Used in the updater for triggering the creation of the new button
                        this.news.data = { upper : currentDatabaseMax, offset : 0 };
                    }
        },

        init : function(obj) {
            this.storeMax(obj.upper); //store the current max row ID based on the upper row boundary of the object.
            this.console.onConsole(obj);
            this.more.update(obj);
            this.updater.init(this.news.update.bind(this.news));
        }

    };

    App.prototype.components = {

        counter : 0, //counter to identify the messages in the DOM tree

        filterFunc : function () { }, //assigned in constructor

        render : function(params) { //Description: Pass an obj in the form { id : ..., data : ...., htmlFunc : function(html) { ... } }

            var str = ""; //html
            var functions = []; //call these functions after returning and creating the elements on the page

            for(var d in params.data) {

                var msg = params.data[d];

                //create the id
                var idString = "message_"+this.counter;
                this.counter++;

                str += "<div data-role='collapsible' name='"+msg.head.name+"' id='"+idString+"'>";

                //Description: Scale the name of the structure so that it does not take up more space on the screen than the maximum permitted size.
                var scale = function(str) {
                    var maxLength = 12.0; //12 characters
                    if((str.length*1.0) < maxLength) {
                        return 1.0;
                    }
                    return maxLength/(str.length*1.0);
                };

                str += "<h3><span style='font-size:"+scale(msg.head.name)+"em'>"+msg.head.name+"</span><span style='float: right; width: 50%; text-align: right'><sup>"+msg.head.datetime+"</sup></span>"+"</h3>";


                str += "<ul data-role='listview' data-inset='true' data-divider-theme='d'>";

                str += "<li data-role='list-divider'>Structure</li>";


                var renderInteger = function(value) {
                    return "<li>Hexadecimal<p class='ui-li-aside'>"+value.toString(16).toUpperCase()+"</p></li>"
                            + "<li>Decimal<p class='ui-li-aside'>"+ value.toString(10).toUpperCase()+"</p></li>";
                };

                var appearanceRenderer = {
                    subCounter : 0,
                    render : function(entryName, obj) {
                        var html = "";
                            html += ((function(name,appearance) {
                                var IDs = {
                                                //Description: ["a,"b","c", ...] -> { a : id("a"), b : id("b"), c : id("c"), ...}
                                                create : function(keys, args) {
                                                    return _.object(keys, _.map(keys, function(k) { return args.idString+"_"+k+"_"+args.subCounter; }));
                                                }
                                            };
                                var args = {idString: idString, subCounter: this.subCounter};
                                var ids = IDs.create(["root","anchor"], args);
                                var component = Components.Appearance.create(idString,obj);
                                functions = functions.concat(_.map(component.controls.all(), function(control) { return {func: control, args: {} }; }));
                                var ret = "";
                                ret += "<ul data-role='listview' data-inset='true' data-divider-theme='d'>";
                                ret += component.view.all();
                                ret += "</ul>";
                                ret += "<div id='"+ids.root+"'>";
                                ret += "<a href='#' id='"+ids.anchor+"' data-role='button'><span style='color:#00DD00'>Item Link</a>";
                                ret += "</div>";
                            var createForms = function(args) {
                                var instance = new Items.Instance.Form(IDs.create(["root", "anchor", "menu", "select", "selectedItemTypeList", "cancelButton", "newButton", "okButton"],args), component);
                                var types = new Items.Types.Form(IDs.create(["root", "menu", "propertyList", "propertyInput", "addPropertyButton", "nameInput", "dataTypes", "okButton", "cancelButton"],args));
                                types.controls.aspects.onComplete = (function(obj) { this.controls.ajaxSelectItemTypes(); }).bind(instance);
                                types.controls.aspects.onClose = (function() {this.controls.ajaxSelectItemTypes(); this.controls.reset(); }).bind(instance);
                                instance.controls.onTransition = (function(){ this.reset(); }).bind(types.controls);
                                instance.controls.registerAnchor();
                            };

                            functions.push({ func: createForms, args: args});
                            this.subCounter++;
                            return ret;
                            }).bind(this))(entryName,obj);
                        return html;
                    }
                };

                var renderCollapsible = function(entryName, value, innerFunction) {
                    var ret = "";
                    ret += "<div data-role='collapsible' data-inset='false' data-mini='true'>"
                        + "<h3>"+entryName+"</h3>"
                        + "<ul data-role='listview' data-inset='true' data-divider-theme='d'>";
                    ret += innerFunction(entryName, value);
                    ret += "</ul>"
                        + "</div>";
                    return ret;
                };

                var renderPrimitive = function(entryName, value) {
                    var ret = "";
                    if(typeof(value)=="number") {

                            ret += renderInteger(value);

                    }else if(typeof(value)=="string") {

                        var tryInt = parseInt(value);

                        if(isNaN(tryInt)) {

                            ret += "<li>"+value+"</li>";

                        } else {

                            ret += renderInteger(tryInt);
                        }

                    } else {

                        ret += "<li>"+typeof(value)+"</li>";

                    }
                    return ret;
                };

                var renderTypes = [ { name: "appearance", onRender: appearanceRenderer.render.bind(appearanceRenderer)}];

                //Display each piece of data in the struct; TODO: Render functions can become more diverse based on the data type;
                //The values could be sent as { type: "UInt32", value : "3" }. This would be more precise.
                var renderData = function(entryName, value) {

                    var ret = "";

                    if(typeof(value)=="object") {

                    var tryRenderType = function(elem) {
                        if(entryName == elem.name) {
                            ret += elem.onRender(entryName, value);
                            return true;
                        }
                        return false;
                     };

                    //TODO: Search render types if object has "type"
                     if(!_.some(renderTypes, tryRenderType)) {
                        for(var e in value) {
                            ret += renderData(e, value[e]);
                        }
                     }

                    } else { //Other no type: must be a primitive
                        ret += renderCollapsible(entryName, value, renderPrimitive);
                    }


                    return ret;
                };

                for(var key in msg.body) {
                    str += renderData(key.toString(), msg.body[key]);
                }



                if(msg.foot.hex != undefined) {


                    str += "<li data-role='list-divider'>Hexadecimal</li>";
                    str += "<li>";

                    /* Render hex array with labels inside list as a set of tables; this should be its own function. */
                    var hexValues = msg.foot.hex.split(/ |\n/); //split at space or new line; "XX XX XX\nXX" -> [XX, XX, XX, XX]
                    hexValues = _.chain(hexValues)
                                    .filter(function(val) { return !(val.length==0); }) //get rid of elements that look like "", caused by spaces & newlines
                                    .map(function(val) { return [{type: "hex", value: val }, {type: "whitespace", value: " "}]; }) //map a value v -> [v]
                                    .value(); //return [ [v1], [v2], ... [vn] ]

                    var colors = ["#FF0000", "#00FF00"]; //the colors are manipulated as a ring; removed from front; added to the back again
                    var subCounter = 0;

                    for(var m in msg.foot.maps) {
                        var subIDString = idString + "_" + subCounter; //identify each part of the buffer, e.g. message_0_0, message_0_1, ...
                        subCounter++;
                        var map = msg.foot.maps[m];
                        var color = colors.shift();
                        hexValues[map.beginIdx].unshift({ type: "html-begin-label", id: subIDString, color: color, endIdx : map.endIdx });
                        hexValues[map.endIdx].push({ type: "html-end-label",  name: map.name});
                        colors.push(color); //ring
                    }

                    if(hexValues.length > 0) {
                        hexValues[0].unshift({type: "html-begin-table"});
                        hexValues[hexValues.length-1].push({type: "html-end-table"});
                    }

                    var insert = function(idx) {
                        for(var j = idx; j > 0; j--) {
                            for(var n in hexValues[j]) {
                                    if(hexValues[j][n].type == "html-begin-label") { //Idx is within a label
                                        var end = hexValues[j][n].endIdx;
                                        var color = hexValues[j][n].color;
                                        var temp = hexValues[end]; //adjust old ending because of the insertion
                                        var name = "";
                                        for(var t in temp) {
                                            if(temp[t].type == "html-end-label") {
                                                    name = temp[t].name;
                                                    hexValues[end][t].name = "...";
                                            }
                                        }
                                        hexValues[idx].push({ type: "html-new-row-inside-label", name: name, color: color});
                                        return;
                                    } else if (hexValues[j][n].type == "html-end-label") {
                                        //Found end first, which implies the insert operation is not taking place inside labelled data
                                        hexValues[idx].push({ type: "html-new-row-outside-label" }); //new row; creates new table as well to allow each row to have different column widths
                                        return;
                                    }
                            }
                        }
                    };

                    //Format lines
                    for(var i = 0; i < hexValues.length; i++) {
                        //Do not insert break at first or last character
                        if(i > 0 && i < (hexValues.length-1) && (i+1) % 16 == 0) {
                            insert(i);
                        }
                    }

                    //Get rid of multi-dimensional arrays, used to insert html where it was needed.
                    hexValues = _.flatten(hexValues); // turn [ [...], ... ] into [ ... , ... ];


                    //apply HTML; [v0,v1,v2,...vn] -> [html(v0),html(v1), html(v2), ... html(vn)]
                    for(var h = 0; h < hexValues.length; h++) {
                        if(hexValues[h].type == "html-begin-label")
                        {

                            var htmlString = "<td><p style='font-size: 0.6em' id='"+hexValues[h].id+"'><span style='color:"+hexValues[h].color+"'>";

                            if((h-1) == 0) {
                                hexValues[h-1] = ""; //replace beginning of table to avoid unused elements e.g. <td><span ... ></span></td>
                                hexValues[h] = "<!--BEGIN--><table border='0' cellspacing='0' cellpadding='0'>" + htmlString;
                             } else {
                                hexValues[h] = "</span></td>" + htmlString;
                            }

                        }else if(hexValues[h].type == "html-end-label") {

                            hexValues[h] = "</span><br/>"+hexValues[h].name+"</p></td><td><span style='font-size: 0.6em'>";

                        } else if(hexValues[h].type == "html-new-row-inside-label") {

                            hexValues[h] = "</span><br/>"+hexValues[h].name+"</p></td></tr></table><table border='0' cellspacing='0' cellpadding='0'><tr><td><p style='font-size:0.6em'><span style='color:"+hexValues[h].color+"'>";

                        } else if(hexValues[h].type == "html-new-row-outside-label") {

                            hexValues[h] = "</span></td></tr></table><table><tr><td><span style='font-size:0.6em'><!--NEW ROW OUTSIDE LABEL-->";

                        } else if(hexValues[h].type == "html-begin-table") {

                            hexValues[h] = "<!--BEGIN--><table border='0' cellspacing='0' cellpadding='0'><td><span style='font-size: 0.6em'>";

                        } else if(hexValues[h].type == "html-end-table") {

                            hexValues[h] = "</span></td></tr></table><!--END-->";

                        } else if (hexValues[h].type == "hex") {

                            hexValues[h] = hexValues[h].value;

                        } else if (hexValues[h].type = "whitespace") {

                            hexValues[h] = hexValues[h].value;

                        }
                    }

                    str += hexValues.join(""); //[html(v0),html(v1), html(v2), ... html(vn)] -> html(v0) + html(v1) + html(v2) + ... + html(vn)
                    str += "</li>";
                }
                str += "</ul>";
                str += "</div>";

            }

            params.htmlFunc(str);
            functions.forEach(function(elem) {
                elem.func(elem.args); //call each function that requested after html is created
            });
        }

    };

  App.prototype.modifiers = {
      appearance : {
         /**
          * Description: Sends a request to the server to log appearance.xml, which creates a component that can be used to create items.
          */
          registerClick : function() {
              $("#page-portal-actions-appearance").click((function(e) {
                  $.ajax({
                          type: "POST",
                          url: "./ajax/modifiers/appearance",
                          cache: false,
                          data: {},
                          dataType: "json"
                         });
              }).bind(this));
          }
      }
    };

    App.prototype.filters = {

        on : [],

        showObj : function(obj) {
            $("[name='"+obj.name+"']").each(function(idx,elem) { $(elem).fadeIn("slow"); });
        },

        hideObj : function(obj) { //hides one obj
            $("[name='"+obj.name+"']").each(function(idx,elem) { $(elem).fadeOut("slow"); });
        },

        hide : function() { //hides all objects in on
            this.on.forEach(this.hideObj);
        },

        //Note: Do NOT delete this function; it can be used as a 'block' action.
        filter : function(messages) { //Description; blocks incoming messages from the server; currently isn't used in favor of 'hide'
            return messages.filter((function(obj) {
                return this.on.every(function(f) {
                    if(f.name == obj.head.name) {
                        return false;
                    }
                });
            }).bind(this));

        },

        render : function(obj) {
            var content = _.template(($("#template-filter-item").html()), obj);
            $("#page-portal-panel-listview").append(content);
            $("#page-portal-panel-listview").listview("refresh");
            $("#page-portal-panel").trigger("create");
            $("#page-portal-panel").trigger("updateLayout");
            var self = this; //two different this references will be used; 'self' refers to app.filters; 'this' refers to the select node;
            $("#"+obj.id).change(function() {
                if($(this).val() == "on") {
                    self.hideObj(obj);
                    self.on.push(obj); //store in turned on filters.
                }else{
                    self.showObj(obj);
                    self.on = self.on.filter(function(a) { if(a.id == obj.id) { return false;  } });
                }
            }); //binds to #filterID
        },

        registerClick : function() {
           $("#page-portal-panel-actions-filter").click((function(e) {
                var filterName = $("#page-portal-panel-input-filter").val().split(" ").join("_"); //name of rendered object to match; e.g. Head
                var filterID = "filter-item-"+filterName;
                var obj = { name: filterName, id: filterID};
                if(obj.name.length > 0 && $("#"+obj.id).length == 0) { //can't add a blank filter; can't add two of the same filters twice.

                    this.render(obj);
                }
           }).bind(this));
        }
    };


    App.prototype.news = {

        renderFunc : function() { }, //Is assigned in the constructor
        hideFunc : function() { },

        dataFunc : function() {}, //Is assigned in the constructor; builds data for an ajax request
        nextData : {}, //Is generated by dataFunc

        isHidden : function() {
            return $("#new-button-div").is(":hidden");
        },

        onNew : function(obj) {

            var editid = "#page-portal-content";

            var tempHTML = $("#new-button-div").html();
            $("#new-button-div").remove();
            this.renderFunc({ id: editid,
                              data: obj.messages,
                              htmlFunc: function(html) { $(editid).prepend(html); }
                            });
            $(editid).prepend("<div id='new-button-div'>"+tempHTML+"</div>"); //re-add the more button at the end
            $(editid).trigger("create"); //apply css
            $("#new-button-div").hide(); //hide when created
            this.hideFunc(); //hides filtered messages
         //   $.mobile.changePage($("#page-portal"), {allowSamePageTransition: true, transition: "none", showLoadMsg : false, reloadPage : true});
        },
        
        onClick : function(e) {
            $("#new-button").unbind("click");
            $("#new-button-div").hide();
            $.ajax({
                    type: "POST",
                    url: "./ajax/console/new",
                    cache: false,
                    data: this.nextData,
                    dataType: "json"
            }).done((function(obj) { this.onNew(obj); }).bind(this));
        },

        update : function() {
            if(this.isHidden()) {
                this.nextData = this.dataFunc(); //prepare next data now; before the record of the max row in the database is updated.
                $("#new-button-div").show();
            //    $("#new-button").click("on");
                $("#new-button").click(this.onClick.bind(this));
            }
        }

    };

    App.prototype.updater = {

        db : { max : 0 },

        ajax : function () {
            return $.ajax({
                            type: "POST",
                            url: "./ajax/console/max",
                            cache: false,
                            dataType: "json"
                          });
        },

        done : function (callback, obj) {
            if(obj.max > this.db.max && this.db.max != 0) {
                callback();
                this.db.max = obj.max;
            }
        },

        init : function (callback) {
            setTimeout((function() {
                console.log("app.updater.update called");
                 this.ajax().done((function(obj) { this.done(callback,obj); this.init(callback); }).bind(this));
            }).bind(this), 2000); //adjust timeout as needed
        }
    };

    App.prototype.login = {

        data : { userid:0, usertype:0, username: "", password: "" },

        complete : false,

        isComplete : function () {  //return bool
            return this.complete;
        },

        callback : function () { }, //called after ajax is done; can be set manually

        ajax : function() {
            this.data.username = $("#input-username").val();
            this.data.password = $("#input-password").val();
            return $.ajax({
                type: "POST",
                url: "./ajax/login",
                data: (this.data),
                cache: false,
                dataType: "json"
            });
        },


        done : function(loginReply) {
            $.mobile.loading( 'hide' );
            this.data.userid = loginReply.userid;
            this.data.usertype = loginReply.usertype;
            this.data.username = loginReply.username;
            this.data.password = loginReply.password;
            if(this.data.userid != 0 && this.data.userid != undefined)
            {
                this.complete = true;
            }
        },

        registerClick : function(obj) {
            this.callback = obj.next;
            $("#btn-login").click(this.onClick.bind(this));
        },

        onClick : function(e) {

            $.mobile.loading( 'show', {
	           text: 'Connecting',
                textVisible: true,
	           theme: 'a',
	           html: ""
            });

            this.ajax().done( (function(loginReply) { this.done(loginReply); this.callback(); }).bind(this) );
        }

    };

    App.prototype.console = {

        renderFunc : function () { }, //set in constructor
        hideFunc : function() { },

        ajax : function () {
           return $.ajax({
                     type: "POST",
                     url: "./ajax/console",
                     cache : false,
                     dataType: "json"
                    });
        },

         onConsole : function(obj) {
            var editid = "#page-portal-content";
      //      alert(obj.messages);
            $.mobile.changePage($("#page-portal"));
            $(editid).empty();
            $(editid).append("<div id='new-button-div'><a href='#' id='new-button' data-role='button'>New</a></div>");
            this.renderFunc({ id: editid,
                              data: obj.messages,
                              htmlFunc: function(html) { $(editid).append(html); }
                            });
            $(editid).append("<div id='more-button-div'><a href='#' id='more-button' data-role='button'>More</a></div>");
            $(editid).trigger("create"); //apply styles
            $("#new-button-div").hide();
            this.hideFunc(); //hides filtered messages

        }

    };

    App.prototype.more = {

        renderFunc : function() { }, //set in the constructor
        hideFunc : function() { },

        queue : [],


        onMore : function(obj) {
            var editid = "#page-portal-content";
        //    alert(obj.messages);
            var tempHTML = $("#more-button-div").html();
            $("#more-button-div").remove();
            this.renderFunc({ id: editid,
                              data: obj.messages,
                              htmlFunc: function(html) { $(editid).append(html); }
                            });
            $(editid).append("<div id='more-button-div'>"+tempHTML+"</div>"); //re-add the more button at the end
            $(editid).trigger("create"); //apply css
            this.hideFunc(); //hides filtered messages
         //   $.mobile.changePage($("#page-portal"), {allowSamePageTransition: true, transition: "none", showLoadMsg : false, reloadPage : true});

        },


        onClick : function(e) {
            $("#more-button").unbind("click"); //disable more clicking until the request is complete; concurrency-related
            this.ajax().done(this.done.bind(this));
        },

        update : function(obj) {
            if(obj.offset >= obj.upper) { //next row to select is non-existent
                $("#more-button-div").hide();
            } else {
                this.queue.push(obj);
                $("#more-button").click(this.onClick.bind(this));
            }
        },

        dataFunc : function() {
            if(this.queue.length == 0) {
                    return {};
            }
            var nextObj = this.queue.shift();
            return nextObj;
        },

        ajax : function() {
           return $.ajax({
                    type: "POST",
                    url: "./ajax/console/more",
                    cache: false,
                    data: this.dataFunc(),
                    dataType: "json"
             });
        },

        done : function(obj) {
            if(!$.isEmptyObject(obj)) {
                this.onMore(obj);
                this.update(obj);
            } else {
                $("#more-button-div").hide();
            }
        }

    };