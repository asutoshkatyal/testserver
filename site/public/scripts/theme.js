// written by Alex Dunn in 2014

(function() {
    "use strict";

    // keyCode/charCode variable
    // in case we want 's' to trigger theme switch
    var keyTheme = 115;

    var themes = ["ucsb", "light", "default"];

    addListener(window, "keypress", function(key) {
        // do this stuff when a key is pressed:

        key = key || window.event;
        var theKey = key.which || key.keyCode;

        if ( theKey === keyTheme ) {
            themeSwitch();
        }

    });

    addListener(window, "load", function() {
        // do this stuff when the `window.load` event fires:

        // set the default theme
        document.documentElement.classList.add(themes[0]);

        var themeButtonText = "<button class=\"themer\">Change Theme (or press the 's' key)</button>";

        // find the first element after <body>:
        var firstElement = document.body.children[0];
        // then make a new div
        var helpDiv = document.createElement("div");
        // and give it a class (if you change the class name, make sure to
        // change it in screen.scss)
        helpDiv.classList.add("themeDiv");
        // and shove it into the top of the page
        document.body.insertBefore(helpDiv, firstElement);
        // and give it some content:
        helpDiv.innerHTML=themeButtonText;

        addListener(helpDiv.children[0], "click", themeSwitch);

        // end window.load event listener
    });

    function themeSwitch() {

        var html = document.documentElement.classList;

        for ( var i = 0; i < themes.length; i++ ) {
            // if we've reached the active theme,
            if ( html.contains(themes[i]) ) {
                // then turn it off,
                html.toggle(themes[i]);
                // if we're NOT at the penultimate theme,
                if ( i < themes.length - 1 ) {
                    // then turn on the next theme
                    html.toggle(themes[i + 1]);
                    break;
                }
                // if we ARE at the penultimate theme, then start over
                else {
                    html.toggle(themes[0]);
                }
            }
        }
    }

    /// add event listener:
    // https://developer.mozilla.org/en-US/docs/DOM/element.addEventListener
    // http://stackoverflow.com/a/1841941/1431858
    /// only `type` argument is quoted (i.e.: window, "load", function())
    function addListener(element, type, response) {
        if (element.addEventListener) {
            element.addEventListener(type, response, false);
        }
        else if (element.attachEvent) {
            element.attachEvent("on" + type, response);
        }
    }

    // var now = new Date().toTimeString();

// end main function
})();
