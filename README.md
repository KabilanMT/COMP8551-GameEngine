# COMP8551-GameEngine
Major Project for COMP8551

## Setup
### Make
In order to run make you will need make for windows found here:</br>
http://gnuwin32.sourceforge.net/packages/make.htm </br>
Note: Just dl and install complete package

## How To Run (Cygwin)
### Running
Navigate to file that contains Makefile then:</br></br>
To build the game
```
make
```
To run the game
```
./engine.exe
```
### Debugging
If when trying to run it says it can't find glfw3.dll, just move the dll file in libs to the same folder as the Makefile. Again not sure why this is a thing.

## CustomScript
### Example
```
<root>
    <variables>
        <int name="health" value="5"/>
        <bool name="isTrue" value="true"/>
        <entity name="robotEntity" value="robot" />
    </variables>

    <start>
        <updateVar name="health" value="4" type="int" />
        <addVar name="health" value="3" type="int" />
    </start>

    <update>
        <ifVarGreater name="health" value="6" type="int">
            <onEntity name="robot">
                <moveEntity x="0.0" y="-9.0" z="0.0"/>
            </onEntity>
        </ifVarGreater>
        <keyPress value="KEY_W">
            <moveEntity x="0.0" y="9.0" z="0.0"/>
        </keyPress>
        <changeSprite name="src/res/textures/Road.png" />
    </update>
    <onCollision>

    </onCollision>
</root>
```
### Script Commands
#### Variables
Stores a variable to be used later. <br /> 
Variable types: int, float, double, string, boolean, and entity. <br />
Reserved variables: <br />
deltaTime (double): the deltatime of the frame. <br />
objectCollision-name (string): set to entity's name collided with. <br />
objectCollision-tag (string): set to entity's tag collided with. <br />   
```
<variables>
    <int name="health" value="2" />
    <float name="timer" value="0.0" />
    <double name="attackTimer" value="2.0" />
    <string name="hello" value="hello world" />
    <bool name="isTrue" value="true" />
    <entity name="entityName" value="entityName" />
<variables>
```
#### Start
Commands to run when the scene starts.
```
<start>
    <!-- Any commands -->
</start>
```
#### Update
Commands to run when the scene updates on each frame.
```
<update>
    <!-- Any commands -->
</update>
```
#### OnCollision
Commands to run when a collision occurs on that object.
```
<onCollision>
    <!-- Any commands -->
</onCollision>
```
#### IfVar
If statement to check if a variable defined is of a particular value.
```
<ifVar name="variableName" value="valueToCompare" type="variableType">
    <!-- Any commands -->
</ifVar>
```
#### IfVarGreater
If statement to check if a variable defined is greater then a particular value.
```
<ifGreaterVar name="variableName" value="valueToCompare" type="variableType">
    <!-- Any commands -->
</ifGreaterVar>
```
#### IfVarLess
If statement to check if a variable defined is less then a particular value.
```
<IfVarLess name="variableName" value="valueToCompare" type="variableType">
    <!-- Any commands -->
</IfVarLess>
```
#### IfGlobalVar
If statement to check if a global variable defined is of a particular value.
```
<ifGlobalVar name="variableName" value="valueToCompare" type="variableType">
    <!-- Any commands -->
</ifGlobalVar>
```
#### IfGlobalVarGreater
If statement to check if a global variable defined is greater then a particular value.
```
<ifGlobalVarGreater name="variableName" value="valueToCompare" type="variableType">
    <!-- Any commands -->
</ifGlobalVarGreater>
```
#### IfGlobalVarLess
If statement to check if a variable defined is less then a particular value.
```
<ifGlobalVarLess name="variableName" value="valueToCompare" type="variableType">
    <!-- Any commands -->
</ifGlobalVarLess>
```
#### KeyPress
When key is pressed do commands. Not all keys work, to check if a key works check the unordered map in the CScriptFunctions namespace.
```
<keyPress value="KEY_E">
    <!-- Any commands -->
</keyPress>
```
#### UpdateVar
Updates a stored variable's value to a new value.
```
<updateVar name="variableName" value="newValue" type="variableType" />
```
#### UpdateGlobalVar
Updates a stored global variable's value to a new value.
```
<updateGlobalVar name="variableName" value="newValue" type="variableType" />
```
#### AddVar, SubVar, MultiVar, DivideVar
Adds, substracts, multiples or divides the stored variable's value by a value.
```
<addVar name="variableName" value="valueToAdd" type="variableType" />
<subVar name="variableName" value="valueToSubtract" type="variableType" />
<!-- Etc. -->
```
#### AddGlobalVar, SubGlobalVar, MultiGlobalVar, DivideGlobalVar
Adds, substracts, multiples or divides the stored global variable's value by a value.
```
<addGlobalVar name="variableName" value="valueToAdd" type="variableType" />
<subGlobalVar name="variableName" value="valueToSubtract" type="variableType" />
<!-- Etc. -->
```
#### ChangeSprite
Changes sprite to new sprite.
```
<changeSprite name="pathToSpriteTexture" />
```
#### OnEntity
Runs commands on stored entity instead of current entity.
```
<onEntity name="entityName">
    <!-- Do commands -->
</onEntity>
```
#### CallFunction
Calls custom function. If done in a onEntity command, will call function in that's entity's custom script. 
```
<callFunction name="yourFunctionName" />

<yourFunctionName>
    <!-- Do commands -->
</yourFunctionName>
```
#### MoveEntity
Move an entity in a direction.
```
<moveEntity x="0.0" y="1.0" z="0.0" />
```
#### moveEntityByVars
Same as moveEntity, but uses the values stored in the variables referenced by x, y, and z, instead of float literals.
```
<moveEntityByVars x="nameOfXFloatVariable" y="nameOfYFloatVariable" z="nameOfZFloatVariable" />
```
#### SetActive
Set the entity's active.
```
<setActive value="true" />
```
#### RemoveEntity
Deletes current entity from scene.
```
<removeEntity />
```
#### MatchEntityPos
Set the entity's position to the current entity's position.
```
<matchEntityPos name="nameOfEntityVariable" />
```
#### DistanceTo
Finds the distance between the current entity and the one referenced by name. Stores the result in the float variable referenced by var.
```
<distanceTo name="entityVariableName" var="floatVariableToStoreIn"/>
```
#### VectorTo
Find the direction from the current entity towards the entity referenced by name. Stores the result in the two float variables referenced by x and y.
```
<vectorTo name="entityVariableName" x="floatVariableToStoreXValueIn" y="floatVariableToStoreYValueIn"/>
```
#### Log
Logs the value to a log file. Due to limitations only can log strings and not variables. 
```
<log value="stringToSendToLogFile" />
```
