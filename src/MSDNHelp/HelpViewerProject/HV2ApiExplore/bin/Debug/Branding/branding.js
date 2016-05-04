var isPrinterFriendly = false;
var userPreferenceLang = "JavaScript";

function setUserPreferenceLang(index)
{
    if (index == 1) {
        userPreferenceLang = "C#";
        return;
    }
    if (index == 2) {
        userPreferenceLang = "C++";
        return;
    }
    if (index == 3) {
        userPreferenceLang = "F#"
        return;
    }
    if (index == 4) {
        userPreferenceLang = "JScript"
        return;
    }
    if (index == 5) {
        userPreferenceLang = "Visual Basic"
        return;
    }
}

function onLoad() 
{
    // Read the query string to check if it's a print request
    var qs = window.location.search.substring(1);

    // Set all the codesnippets to focus the user preferred tab
    // As of now, we have fixed index value for a specific lang but we might make it dynamic, so passing both lang and index value.
    // find index of the lang in codesnippet collection
    var index = getIndexFromDevLang(userPreferenceLang);
    setCodesnippetLang(userPreferenceLang, index);

    // if it's a print request then set isPrinterFriendly to true    
    if (qs.toLowerCase().indexOf('print=true') != -1) 
    {
        isPrinterFriendly = true;
    }
    
    // always show expanded for printer friendly pages.
    if (isPrinterFriendly)
    {
        // loop through the object and expand before printing.
        for (var lstMember in listCollapsibleArea) {
            var elem = document.getElementById(listCollapsibleArea[lstMember]);
            var content = document.getElementById(listCollapsibleArea[lstMember] + '_c');
            if (elem && content) {
                var img = document.getElementById(listCollapsibleArea[lstMember] + '_img');
                img.className = 'cl_CollapsibleArea_expanding';
                content.className = 'sectionblock';
            }
        }
    }

    // support highcontrast for VS logo
    // Check for high contrast mode
    if (isHighContrast()) 
    {
        onHighContrast(isBlackBackground());
    }
}

/****************************************************Codesnippet functionality*******************************************************/
// we stored the ids of code snippets of same pages so that we can do interaction between them when tab are selected
var snippetIdSets = new Array();

var allLanguageTagSets = new Array();

// We call this function from Redering-writeStandaloneCodeSnippet & writeCodeSnippet functions.
// The following method will fill up the snippetIdSets with codesnippet ids on the page and snippetIdSets with devlangs used in snippet collections, which will be used in LST control.
function addSpecificTextLanguageTagSet(codesnippetid) 
{
    var i = 1;
    while (i < 7) {
        var snippetObj = document.getElementById(codesnippetid + "_tab" + i);
        if (snippetObj == null) break;

        var tagSet = getDevLangFromCodeSnippet(snippetObj.innerHTML);
        var insert = true;
        var j = 0;
        while (j < allLanguageTagSets.length) 
        {
            if (allLanguageTagSets[j] == tagSet) 
            {
                insert = false;
                break;
            }
            j++;
        }
        if (insert) allLanguageTagSets.push(tagSet);
        i++;
    }
    snippetIdSets.push(codesnippetid);
}

function getIndexFromDevLang(lang)
{
    var temp = lang.toLowerCase().replace(" ", "");
    if (temp.indexOf("javascript") != -1)
        return 1;
    if ((temp.indexOf("csharp") != -1) || (temp.indexOf("c#") != -1))
        return 2;
    if ((temp.indexOf("cplusplus") != -1) || (temp.indexOf("visualc++") != -1) || (temp.indexOf("c++") != -1))
        return 3;
    if ((temp.indexOf("f#") != -1) || (temp.indexOf("fs") != -1) || (temp.indexOf("fsharp") != -1))
        return 4;
    if (temp.indexOf("jscript") != -1)
        return 5;
    if (temp.indexOf("visualbasic") != -1 || temp.indexOf("vb"))
        return 6;
}

// Functions called from codesnippet.xslt
function ChangeTab(objid, lang, index, snippetCount) 
{
    setCodesnippetLang(lang, index);
    userPreferenceLang = lang;

    var currentLang = getDevLangFromCodeSnippet(lang);
    updateLST(currentLang);
}

var userPreferenceLangExists = false;
var firstSelectedLang = null;

function setCodesnippetLang(lang, index)
{
    i = 0;
    while (i < snippetIdSets.length) 
    {
        // find if it's a solo snippet and if it a solo snippet then skip processing.
        var _tempSnippetCount = 6;
        if (document.getElementById(snippetIdSets[i] + "_tab5") == null) 
        {
            i++;
            continue;
        }

        // check if the selected snippet is present in the snippet collection and if not present then set the index to 1
        if (document.getElementById(snippetIdSets[i] + "_tab" + index) == null) 
        {
            index = 1;
        }

        setCurrentLang(snippetIdSets[i], lang, index, _tempSnippetCount);
        i++;       
    }

    // Adjust the LST to the first code snippet group selected language if there is no active user prefered code snippet tab exists.
    if (userPreferenceLangExists == false && firstSelectedLang != null){
        var currentLang = getDevLangFromCodeSnippet(firstSelectedLang);
        updateLST(currentLang);
    }
}

function setCurrentLang(objid, lang, index, snippetCount) 
{
    var _tab = document.getElementById(objid + "_tab" + index);
    if (_tab != null) {
        if (document.getElementById(objid + "_tab" + index).innerHTML.match("javascript:") == null) {
        
            //Select left most tab as fallback
            var i = 1;
            while (i < snippetCount + 1) 
            {
                if (!document.getElementById(objid + "_tab" + i).disabled) 
                {
                    setCurrentLang(objid, document.getElementById(objid + "_tab" + i).innerHTML, i, snippetCount);
                    return;
                }
                i++;
            }
            return;
        }
        var langText = _tab.innerHTML;
        if (langText.indexOf(lang) != -1) 
        {
            i = 1;
            while (i < snippetCount + 1) 
            {
                var tabtemp = document.getElementById(objid + "_tab" + i);
                if (tabtemp != null) 
                {
                    if (isPrinterFriendly) 
                    {
                        tabtemp.style.display = 'none';
                    }
                    else 
                    {
                        if (tabtemp.className == "OH_CodeSnippetContainerTabActive")
                            tabtemp.className = "OH_CodeSnippetContainerTabFirst";

                        if (tabtemp.className == "OH_CodeSnippetContainerTabActiveNotFirst")
                            tabtemp.className = "OH_CodeSnippetContainerTab";
                    }

                }
                // update the selected lang style from display none to display block.
                document.getElementById(objid + "_tab" + index).style.display = 'block';

                var codetemp = document.getElementById(objid + "_code_Div" + i);
                if (codetemp != null) 
                {
                    if (codetemp.style.display != 'none')
                        codetemp.style.display = 'none';
                }
                i++;
            }
            document.getElementById(objid + "_tab" + index).className = "OH_CodeSnippetContainerTabActive";
            if (index != 1)
                document.getElementById(objid + "_tab" + index).className = "OH_CodeSnippetContainerTabActiveNotFirst";

            document.getElementById(objid + '_code_Div' + index).style.display = 'block';

            // change the css of the first/last image div according the current selected tab
            // if the first tab is selected
            if (index == 1)
                document.getElementById(objid + "_tabs").firstChild.className = "OH_CodeSnippetContainerTabLeftActive";
            else 
            {
                if (document.getElementById(objid + "_tabs").firstChild.className != "OH_CodeSnippetContainerTabLeftDisabled")
                    document.getElementById(objid + "_tabs").firstChild.className = "OH_CodeSnippetContainerTabLeft";
            }

            // if the last tab is selected
            if (index == snippetCount)
                document.getElementById(objid + "_tabs").lastChild.className = "OH_CodeSnippetContainerTabRightActive";
            else 
            {
                if (document.getElementById(objid + "_tabs").lastChild.className != "OH_CodeSnippetContainerTabRightDisabled")
                    document.getElementById(objid + "_tabs").lastChild.className = "OH_CodeSnippetContainerTabRight";
            }

            // show copy code button if EnableCopyCode is set to true 
            if (document.getElementById(objid + "_tab" + index).getAttribute("EnableCopyCode") == "true")
            {
                document.getElementById(objid + "_copycode").style.display = 'inline';
            }
            else 
            {
                document.getElementById(objid + "_copycode").style.display = 'none';
            }
        }

        // record the very first code snippet selected lang
        if (firstSelectedLang == null){
            firstSelectedLang = _tab.innerText;
        }

        if (_tab.innerText == userPreferenceLang) {
            userPreferenceLangExists = true;
        }
    }
}

function CopyToClipboard(objid, snippetCount) 
{
    var contentid;
    var i = 1;
    while (i <= snippetCount) 
    {
        var obj = document.getElementById(objid + '_code_Div' + i);
        if ((obj != null) && (obj.style.display != 'none') && (document.getElementById(objid + '_code_Plain_Div' + i).innerText != '')) 
        {
            contentid = objid + '_code_Plain_Div' + i;
            break;
        }

        obj = document.getElementById(objid + '_code_Plain_Div' + i);
        if ((obj != null) && (obj.style.display != 'none') && (document.getElementById(objid + '_code_Plain_Div' + i).innerText != '')) 
        {
            contentid = objid + '_code_Plain_Div' + i;
            break;
        }
        i++;
    }
    if (contentid == null) return;
    if (window.clipboardData) 
    {
        try 
        {
            window.clipboardData.setData("Text", document.getElementById(contentid).innerText); 
        }
        catch (e) 
        {
            alert("Permission denied. Enable copying to the clipboard.");
        }
    }
    else 
    {
        return;
    }
}

/****************************************************CollasibleArea functionality*******************************************************/

// write all the collapsiblecontrol object into this list.
var listCollapsibleArea = new Array();
function addToCollapsibleControlSet(id) 
{
    listCollapsibleArea.push(id);
}

// Used by collapsibleArea
function CA_Click(id, ExpandName, CollapsName) 
{
    var elem = document.getElementById(id);
    var img = document.getElementById(id + '_img');
    var content = document.getElementById(id + '_c');
    if (elem && content) 
    {
        if (content.className == 'sectionblock') 
        {
            img.className = 'cl_CollapsibleArea_collapsing';
            elem.title = ExpandName;
            content.className = 'sectionnone';
        }
        else 
        {
            img.className = 'cl_CollapsibleArea_expanding';
            elem.title = CollapsName;
            content.className = 'sectionblock';
        }
    }
};

/****************************************************LST functionality*******************************************************/

// we store the ids of LST control as dictionary object key values, so that we can get access to them and update when user changes to a different programming language. 
// The values of this dictioanry objects are ';' separated languagespecific attributes of the mtps:languagespecific control in the content.
// This function is called from LanguageSpecificText.xslt
var lanSpecTextIdSet = new Object();
function addToLanSpecTextIdSet(id) 
{
    var key = id.split("?")[0];
    var value = id.split("?")[1];
    lanSpecTextIdSet[key] = value;
}

// The function executes on OnLoad event and Changetab action on Code snippets.
// The function parameter changeLang is the user choosen programming language, VB is used as default language if the app runs for the fist time.
// this function iterates through the 'lanSpecTextIdSet' dictionary object to update the node value of the LST span tag per user's choosen programming language.
function updateLST(currentLang) 
{
    if (lanSpecTextIdSet == null)
        return;
    for (var lstMember in lanSpecTextIdSet) 
    {
        var devLangSpan = document.getElementById(lstMember);
        if (devLangSpan != null) 
        {
            // There is a carriage return before the LST control in the content, so the replace function below is used to trim the white space(s) at the end of the previous node of the current LST node.
            if (devLangSpan.previousSibling != null && devLangSpan.previousSibling.nodeValue != null) devLangSpan.previousSibling.nodeValue = devLangSpan.previousSibling.nodeValue.replace(/\s+$/, "");
            var langs = lanSpecTextIdSet[lstMember].split("|");
            var k = 0;
            while (k < langs.length) 
            {
                if (currentLang == langs[k].split("=")[0])
                {
                    devLangSpan.innerHTML = langs[k].split("=")[1];
                    break;
                }
                k++;
            }
        }
    }
}

// We only care abour vb, cpp and cs to set LST values
function getDevLangFromCodeSnippet(lang) 
{
    var tagSet = "nu";
    if (lang != null)
    {
        var temp = lang.toLowerCase().replace(" ", "");
        if (temp.indexOf("visualbasic") != -1)
            tagSet = "vb";
        if ((temp.indexOf("csharp") != -1) || (temp.indexOf("c#") != -1))
            tagSet = "cs";
        if ((temp.indexOf("cplusplus") != -1) || (temp.indexOf("visualc++") != -1) || (temp.indexOf("c++") != -1))
            tagSet = "cpp";
    }
    return tagSet;
}

/****************************************************HighContrast support for VSlogo*******************************************************/

// Called to determine if background is black
// Only accurate when in high constrast mode
function isBlackBackground() 
{
    var color = '';
    if (document.body.currentStyle) 
    { 
        color = document.body.currentStyle.backgroundColor;
    }
    if (color == 'rgb(0, 0, 0)' || color == '#000000') 
    {
        return true;
    }

    return false;
}

// We use a colored span to detect high contrast mode
function isHighContrast() 
{
    var elem = document.getElementById('HCColorTest');
    if (elem) 
    {
        // Set SPAN text color - will not be applied if in contrast mode
        elem.style.color = '#ff00ff';
        if (elem.currentStyle) 
        {
            if (elem.currentStyle.color != '#ff00ff') 
            {
               return true;
            }
        }
    }

    return false;
}

// Called when high constrast is detected
function onHighContrast(black) 
{
    if (black) 
    { 
        // Black background, so use alternative images
        // VS logo
        var logo = document.getElementById('VSLogo');
        if (logo) 
        {
            var logoHC = document.getElementById('VSLogoHC');
            if (logoHC) 
            {
                logo.style.display = 'none';
                logoHC.style.display = '';
            }
        }       
    }
}

/****************************************************MultiMedia functionality*******************************************************/
// these are arrays to support multiple play controls on the page
var playControls = new Array();
var captionStyles = new Array();
var captions = new Array();
var currentSubtitle = new Array();
var captionsOn = new Array();

// caption info container object
function caption(begin, end, text, style) {
    this.begin = begin;
    this.end = end;
    this.text = text;
    this.style = style;
}

function findAllMediaControls(normalizedId) {
    var foundPos = -1;
    var ttsPos = -1;

    // get all the video and audio tags on the page
    var tts = document.getElementsByTagName("video");
    ttsArr = [];
    for (var i = 0, n; n = tts[i]; ++i) ttsArr.push(n);
    tts = document.getElementsByTagName("audio");
    for (var i = 0, n; n = tts[i]; ++i) ttsArr.push(n);

    for (var i = 0; i < ttsArr.length; i++) {
        playControls[i] = ttsArr[i].id.toLowerCase();
        captions[i] = new Array();
        captionStyles[i] = {};
        currentSubtitle[i] = -1;
        captionsOn[i] = -1;
    }

    return foundPos;
}

function getActivePlayer(normalizedId) {
    var foundPos = -1;
    if (playControls.length == 0) {
        foundPos = findAllMediaControls(normalizedId);
    }

    for (var i = 0; i < playControls.length; i++) {
        if (playControls[i] == normalizedId) {
            foundPos = i;
            break;
        }
    }

    return foundPos;
}

function captionsOnOff(id) {
    var foundPos = getActivePlayer(id.toLowerCase());

    if (foundPos > -1) {
        captionsOn[foundPos] = captionsOn[foundPos] * -1;
        if (captionsOn[foundPos] == -1) {
            document.getElementById(id + "_subtitleText").innerHTML = "";
            document.getElementById(id + "_CCon").src = document.getElementById(id + "_CCon").src.replace("ccon.png", "ccoff.png");
        }
        else
            document.getElementById(id + "_CCon").src = document.getElementById(id + "_CCon").src.replace("ccoff.png", "ccon.png");
    }
}

// convert string time to seconds
function toSeconds(t) {
    var s = 0.0;
    if (t) {
        var p = t.split(':');
        for (i = 0; i < p.length; i++) {
            s = s * 60 + parseFloat(p[i].replace(',', '.'));
        }
    }
    return s;
}

function getAllComments(node) {
    exp = "<" + "!" + "--" + "([\\s\\S]+?)" + "--" + ">";
    rexComment = new RegExp(exp, "g");
    var result = [];
    var e;
    while (e = rexComment.exec(node.innerHTML)) {
        result.push(e[1]);
    }

    return result;
}

function styleRectify(styleName, styleValue) {
    var newStyle = "";
    switch (styleName.substring(4).toLowerCase()) {
        case "fontsize":
            newStyle = "font-size:" + styleValue + "px";
            break;
        case "fontweight":
            newStyle = "font-weight:" + styleValue;
            break;
        case "fontstyle":
            newStyle = "font-style:" + styleValue;
            break;
        case "fontfamily":
            newStyle = "font-family:" + styleValue;
            break;
        case "textdecoration":
            newStyle = "text-decoration:" + styleValue;
            break;
        default:
            newStyle = styleName.substring(4) + ":" + styleValue;
    }

    return newStyle;
}

function showCC(id) {
    var foundPos = getActivePlayer(id.toLowerCase());
    document.getElementById(id + "_CCon").style.setAttribute("display", "block");

    var controlId = document.getElementById(id + "_group");

    // this is safe as identical event handlers are discarded
    controlId.addEventListener('mouseout', function hideCC(e) {
        if (e.toElement != null && e.toElement.id != id + "_CCon")
            document.getElementById(id + "_CCon").style.setAttribute("display", "none");
    }, true);
}

// load the DFXP content from the <tt> node
function loadDFXP(id) {
    var normalizedId = id.toLowerCase();
    var foundPos = -1;

    foundPos = getActivePlayer(normalizedId);

    var comments = getAllComments(document.getElementById(id)/*ttsArr[ttsPos]*/);

    // process the style lines
    parser = new DOMParser();
    var ttsNode = parser.parseFromString(comments[0].trim(), "text/xml");
    x = ttsNode.getElementsByTagName("style");
    for (var count = 0; count < x.length; count++) {
        var newStyle = "";
        for (var i = 0; i < x[count].attributes.length; i++) {
            if (x[count].attributes[i].name != "id")
                newStyle = newStyle + styleRectify(x[count].attributes[i].name, x[count].attributes[i].value) + ";";
        }
        captionStyles[foundPos][x[count].attributes["id"].value.toLowerCase()] = newStyle;
    }

    // process the caption lines
    x = ttsNode.getElementsByTagName("p");
    for (var count = 0; count < x.length; count++) {
        var content = "";
        // code handles the use of line break tags
        for (var i = 0; i < x[count].childNodes.length; i++) {
            if (x[count].childNodes[i].nodeType == 3)
                content = content + x[count].childNodes[i].textContent;
            else if (x[count].childNodes[i].nodeType == 1)
                content = content + "<" + x[count].childNodes[i].nodeName + "/>";
        }

        newCaption = new caption(toSeconds(x[count].getAttribute("begin")), toSeconds(x[count].getAttribute("end")), content, x[count].getAttribute("style"));
        captions[foundPos][count] = newCaption;
    }

    return foundPos;
}

// blat out the subtitles as the time rolls by
function subtitle(id) {
    // add cc button
    var foundPos = loadDFXP(id);
    var subtitle = -1
    var ival = setInterval(function () {
        if (captionsOn[foundPos] > 0) {
            var currentTime = document.getElementById(id).currentTime;
            if (currentTime != undefined) {
                for (var i = 0; i < captions[foundPos].length; i++) {
                    if (captions[foundPos][i].begin > currentTime) {
                        break;
                    }
                    subtitle = i;
                }
                if (subtitle != -1) {
                    if (currentSubtitle[foundPos] != subtitle) {
                        document.getElementById(id + "_subtitleText").innerHTML = "<span style=\"" + captionStyles[foundPos][captions[foundPos][subtitle].style] + "\">" + captions[foundPos][subtitle].text + "</span>";
                        currentSubtitle[foundPos] = subtitle;
                    }
                    else {
                        if (captions[foundPos][subtitle].end < currentTime) {
                            document.getElementById(id + "_subtitleText").innerHTML = "";
                        }
                    }
                }
            }
        }
    }, 100);
}