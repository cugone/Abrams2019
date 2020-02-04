This is a living document and can change at any time.

# Contributing to the project
We welcome any contributions you may have to the project.

Current areas lacking in features are:
 - Linux OS support
 - Physics Subsystem
 - OpenGL Renderer

If you'd like to contribute, have a question, or would like to submit a feature request, please [open an Issue](https://github.com/cugone/Abrams2019/issues/new) first so it may be discussed before any major work is done. You don't want to spend a lot of time on a feature only to have it rejected.

Please consider and follow the S&P below when making contributions.
 
## Coding Standards and Practices with Style Guide

This project follows a modified version of the [JSF-AV](http://www.stroustrup.com/JSF-AV-rules.pdf) rules and its descriptors:
 - Rules containing **may** are optional. They are not required and are a weak suggestion.
 - Rules containing **should** are advisory. They strongly suggest the way of doing things.
 - Rules containing **will** are intended to be mandatory. These rules are expected to have exceptions.
 - Rules containing **shall** are strictly mandatory. PRs breaking a **shall** rule are immediately rejected.

### Tabs vs Spaces
#### Rule 1
Tabs **will not** be used as a whitespace separator.

**Rationale:** Readability.

Tabs are system-dependent. Spaces are not. What is readable on your machine becomes an unreadable mess on a different computer when using tabs.

**Exceptions:**
 - Strings intended to be written to a file, parsed by XML, or input to a shader compiler **shall not** contain tabs or the `'\t'` character.
 - Strings intended for developer-only output such as outputting to Visual Studio's Output Window or formatting an error message **may** contain the `'\t'` character.
 - Specifications documenting the use of the `'\t'` character as a requirement **should** be followed. E.g. parsing HTTP header network packets
 
### Braces
#### Rule 2
Braces **shall** follow a variant of the One True Brace Style.

This is a modified K&R style (sometimes called Egyptian) where the opening brace is on the same line as the function signature or control block keyword and the closing brace is on its own line:
```
void foo() {

    {
        int some_local_variable = 0;
    }

    while(...) {
        if(...) {
        } else if(...) {
        } else {
        }
    }
}
```

**Rationale:**

Readability and Style consistency.

Scope blocks defined in this way can easily be converted to functions.

**Exceptions:**
 - Lambdas that are inline or very short **may** be on the same line:
 ```
 FunctionCallWithLambdaArgument([](...) {...});
 ```

#### Rule 3
Control blocks **shall** always be enclosed by braces even if the enclosing block is empty.

**Rationale:** Readability.

#### Rule 4
Empty control blocks **should** contain a comment that it is empty on purpose.
```
class Foo {
    public:
        virtual ~Foo() = 0;
};
//...
Foo::~Foo() {
    /* DO NOTHING */
}
```

**Rationale:** Readability

**Exceptions:**
 - Short Lambdas blocks **may** be empty.

#### Rule 5
Control blocks **shall** contain a space before the opening brace.

### Comments
#### Rule 6
Code **should** be readable and understandable without comments.

**Rationale:** Comments and function documentation go stale very quickly.

**Exceptions:**
 - Code **shall** document exceptions to rules or potentially confusing algorithms.

#### Rule 7
Multi-line comments **should** use C++-style comments:
```
//This is a comment
//that spans multiple lines
```

**Rationale:** C-style comments cannot be nested.

**Exceptions:**
 - Block comment separators within a file or file headers **may** use C-style comment blocks:
 ```
 /***************************************
 *  THIS FILE INTENTIONALLY LEFT BLANK  *
 ****************************************/
 ```
 ```
 /*
 Copyright and Licensing text...
 
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Phasellus eget euismod mi, nec accumsan arcu. Sed tristique molestie tellus eget 
commodo. Aenean vel sem elit. Morbi eu neque a sem fringilla maximus non vel arcu. Proin congue quam quam, ut placerat urna 
vulputate a. Aliquam turpis odio, tempus ut placerat ut, fermentum sed ligula. Cras eros sem, ornare id sollicitudin vel, consequat lacinia 
ipsum. Ut varius nisi a ante tincidunt pharetra. Sed at aliquet lacus. Phasellus dolor purus, gravida sed congue non, posuere in purus. 
Donec id velit congue leo feugiat eleifend vitae quis libero. Integer luctus auctor nisi, vitae porta turpis molestie eu. Suspendisse purus 
tortor, commodo nec eros vitae, pretium molestie leo. Vivamus convallis ex dictum mi blandit lacinia.

Ut arcu ligula, finibus ac sapien id, viverra porta velit. Quisque commodo neque et libero porttitor, eget interdum erat mattis. Maecenas 
aliquam, eros eget vehicula elementum, odio lorem fringilla metus, ut fermentum sapien ante eu arcu. Vestibulum pharetra, nibh porta 
placerat laoreet, massa urna viverra neque, eget pulvinar odio ipsum vitae neque. Aenean pretium lectus nunc, egestas pharetra urna 
dapibus vel. Etiam at eleifend felis, nec volutpat nunc. Sed a nisi lacus. Mauris fermentum erat nec vulputate convallis. Donec sit amet 
ornare orci. Praesent augue orci, ullamcorper eget lacus ac, elementum fringilla neque.
*/
 ```

#### Rule 8
C++-style comments **will not** contain a leading space between the comment designator and the comment.

**Rationale:** Style

**Exceptions**
 - Code **should** try to remain consistent within a single file.

### Indentation
#### Rule 9
All indentation **shall** be 4 spaces.

**Rationale:** Style

### Parenthesis
#### Rule 10
Control blocks **will not** contain a leading space between the control block keyword and the opening parenthesis:
```
if(a) {
    //...
}
[]() {}
```
**Rationale:** Style

**Exceptions:**
 - Code **should** try to remain consistent within a single file.

#### Rule 11
Parenthesis **may** be used to override order of operations even if the order is unchanged.

**Rationale:** Readability

