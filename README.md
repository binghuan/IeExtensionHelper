# IE Extension Helper

<img src="Icon.png" width="auto" height="240px"><br/>

## Description

IE Extension Helper is a development tool designed to make it easier for web developers to create Microsoft Internet Explorer extensions. Unlike Chrome, Firefox, and Safari, which provide convenient extension frameworks that allow developers to use HTML5 and web technologies, Internet Explorer traditionally required developers to use native Windows programming languages like C++/C or C#.

This extension helper bridges that gap by providing a framework that allows web developers to create IE extensions using familiar web technologies, significantly lowering the barrier to entry and reducing implementation complexity.

## Purpose

Web developers are typically proficient in HTML5, CSS, and JavaScript, but may not be familiar with C++/C or C# programming languages. While other browsers (Chrome, Firefox, Safari) provide extension APIs that let developers focus on web technologies, Internet Explorer has historically required native Windows development skills.

IE Extension Helper solves this problem by:
- Eliminating the need to learn C++/C or C# for IE extension development
- Providing a Chrome/Firefox/Safari-like extension framework for IE
- Allowing developers to use their existing web development skills
- Reducing development time and complexity

## How It Works

Developers only need to use the pre-built `IeExtension.dll` along with a `msiemanifest.json` configuration file to achieve similar functionality to Chrome, Firefox, or Safari extension frameworks.

## Installation Structure

To publish an IE extension using this helper, create an installer that places files in the following structure:

```
C:\Program Files\MyIeExtension\
├── icon.ico              // Extension icon
├── IeExtension.dll        // Extension helper library
├── msiemanifest.json      // Extension manifest
├── ContentScript.js       // Injected code into web content
├── Background.js          // Extension background handler
└── Popover.js            // Code for popover page after clicking toolbar button
```

## Configuration

### Sample msiemanifest.json

```json
{
   "background": {
      "debug": false,
      "clsid": "{0CA63DE6-4947-4096-959C-002FEB7C2F90}",
      "name": "BH_BHO",
      "page": "http://127.0.0.1:8000/hello.html"
   },
   "browser_action": {
      "debug": false,
      "clsid": "{25F07CA0-787A-4770-B329-941EB72FDA05}",
      "default_icon": "data/images/Icon-16.png",
      "default_popup": "http://127.0.0.1:8000/hello.html",
      "default_title": "__MSG_extTitle__",
      "height": 600,
      "icon": "icon_64.icon",
      "name": "BH_Toolbar",
      "width": 400
   },
   "content_scripts": {
      "end": {
         "scripts": ["C:\\Users\\binghuan\\content_script.js"]
      }
   },
   "default_locale": "en",
   "description": "__MSG_extDescription__",
   "extension_id": "aDkQEysUQM",
   "name": "__MSG_extName__",
   "version": "2.0"
}
```

## API Reference

### Message Passing

#### Background to ContentScript
```javascript
// Send message
window.external.dispatchMessage2ContentScript("NAME", "MESSAGE");
windowExternalDispatchMessage2ContentScript("NAME", "MESSAGE" or OBJECT);

// Receive message in Content Script
onIeExtensionMsgContentScriptReceive({name:"", message:""})
```

#### ContentScript to Background
```javascript
// Send message
windowExternalDispatchMessage2Background("NAME", "MESSAGE" or OBJECT);

// Receive message in Background
onIeExtensionMsgBackgroundReceive({tabId:"", name:"", message:"", senderUrl: ""})
```

#### Popover to Background
```javascript
// Get references
window.external.getBackgroundPage(); // Returns background window
window.external.getPopoverPage();    // Returns popover window
```

### Event Callbacks

```javascript
onIeExtensionEventTabOpen({id: integer})
onIeExtensionEventTabClose({id: integer})
onIeExtensionEventTabActivate({id: integer})
onIeExtensionEventTabInactivate({id: integer})
onIeExtensionEventPopover()
```

### Browser Extension API

```javascript
// Tab management
window.external.openNewTab(URL);
window.external.getActiveTab(); // Returns {url, id, title}
window.external.setActiveTabUrl(String URL);
window.external.getSelfTab(); // Returns {url, id, title}
window.external.setSelfTabUrl(String URL);
window.external.getTabCounter(); // Returns tab count

// Popover control
window.external.isPopoverVisible(); // Returns true/false
window.external.setPopoverVisible();
window.external.setPopoverInvisible();

// Script execution
window.external.executeScriptInTab(String codeText);

// Shared preferences (global variables across IE tabs)
window.external.setSharedPreferences(string key, string value);
window.external.getSharedPreferences(string key, string defaultValue);
window.external.getSharedPreferences(string key);

// Debug utility
window.external.outputDebugString(String);
```

## Development Status

⚠️ **Note**: This extension helper is still in testing stage and might be buggy!

## Requirements

- Microsoft Internet Explorer
- Windows operating system
- Visual Studio (for building from source)

## Building from Source

This project uses Visual Studio and includes:
- C++ Browser Helper Object (BHO)
- Extension registration utilities
- JSON parsing library (jsoncpp)

## Links

- **Blog Post**: [IE Extension Helper](http://studiobinghuan.blogspot.tw/2013/11/ie-extension-helper.html)
- **Author's Blog**: [Studio Bing-Huan](http://studiobinghuan.blogspot.tw/?view=flipcard)
- **Alternative Blog**: [BH Talk](http://bhtalk.blogspot.tw/)

## Contributing

This project was created to help web developers build IE extensions more easily. Contributions and feedback are welcome to improve the framework and fix any existing issues.