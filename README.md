# H2Analyst
 Data visualization and analysis tool, built for Forze Hydrogen Racing.

## Installation Instructions
In order to install the plotter go to [releases](https://github.com/StichtingFormulaZeroTeamDelft/H2Analyst/releases) page and download the installer for the latest release. Then run the downloaded installer to install the plotter.

To install the project for development see the section on [Local Development Setup](#local-development-setup).

## Local Development Setup
The instructions for a local setup of the project are based on Visual Studio 2022 (Community)\
However the project should be compatible with any version of Visual Studio $\geq$ 2017

### Installing Qt
- Download the Qt installer [here](https://www.qt.io/download-qt-installer-oss?hsCtaTracking=99d9dd4f-5681-48d2-b096-470725510d34%7C074ddad0-fdef-4e53-8aa8-5e8a876d6ab4)
- Launch the installer
- In the **Installation Folder** section select only **Custom Installation**
- Under **Qt** select the following version: **Qt 6.5.3**
- Under **Qt->Developer and Designer Tools** select **Qt Installer Framework 4.7**
- OPTIONAL: Disable the options for Qt Design Studio and Qt Creator (under Developer and Designer Tools)
- Create an environment variable named **QtInstall** pointing to the Qt installation folder (only required when building installers)

### Visual Studio 2022 Setup
- Start Visual Studio 2022
- Open the _.sln_ file from the cloned repository
- Select **Extensions->Manage Extensions**
- Search for **Qt Visual Studio Tools** and download the extension
- Select **Extensions->Qt VS Tools->Qt Versions**
- Add a new Qt version and set the path to your Qt installation folder
  - The correct executable can be found in **Qt->6.5.3->msvc2019_64->bin->qmake.exe**
- Restart Visual Studio and open the project to make sure all changes are applied correctly
- Create an environment variable named **VCINSTALLDIR** pointing to the **VC** folder in the Visual Studio installation folder (only required when building installers)

### Finishing up
Lastly make sure that all the git submodules are pulled aswell.\
Do this by running `git submodule update --init --recursive -f` in the repo folder on your machine.\
Ater following these steps verify that the everything is installed correctly by starting the project (**Ctrl+F5**) in Visual Studio 2022.

## Creating Installers
To create an installer for a new release follow these steps:
- Build the project in the release configuration
- Run the **build_installer.bat** script in the **installer** folder

## Code Structure

The code for this application is structured in the following way:

- **Core**
  - **H2Analyst**  
    Root of the application
  - **Namespace**  
    The namespace contains all constant definitions that are used throughout the application.
  - **DataStore**  
    Back-end of data management within the application. Users interact with the DataStore through the DataPanel.
  - **DataStructures**  
    Definition of the data structures used throughout the application to store the loaded data.
  - **SettingsManager**  
    The settings manager takes care of reading and writing to external settings files.
- **Widgets**
  - **ControlPanel**  
    The ControlPanel is the widget that contains buttons and other control elements to interact with the application.
  - **DataPanel**  
    The DataPanel is the front-end that allows users to interact with loaded data.
    - **TreeView**  
        The TreeView is the main method of presenting the data in the DataStore to the user. It generates a folder structure based on the name of the datasets with systems and subsystems. 
  - **PlotManager**  
    This widget takes care of the creation and placement of plots in the application.
    - **Plot**
      - **Crosshairs**  
        PlotCrosshairs replace the cursor when hovering a plot. They show a horizontal and vertical line, intersecting the cursor, with labels displaying the position on the axis. This is handy when quickly scanning data and checking the values of data at specific times by hand.
      - **Rubberband**  
        The Rubberband is a rectangle that can be dragged to define an area in a plot to zoom towards. It can also snap to horizontal or vertical zooming when the aspect ratio of the rectangle is small or when holding down a key.
      - **TimeCursor**  
        The TimeCursor can be placed at a specific timestamp and indicates the values of all datasets shown at this moment. Its position is synchronized between all active plots to allow the user to check all data values at the same moment. 
  - **Dialogs**
    - **Standards**  
        Standard dialogs are used to do simple things like display a message or ask a yes/no question.
    - **PlotLayout**  
        The PlotLayout dialog is used to set the desired layout of plots.
- **Utilities**
  - **Parsers**  
    Parsers are used to load data of various types by converting them to the data structures used by H2Analyst.
    - **IntCanLog**  
      IntCanLog files are generated by the Forze 8.
  - **DataPopulator**  
    The DataPopulator is a utility that allows fast loading of IntCanLog data by offloading the sorting of messages per dataset to a separate thread. It also features a priority list that can be used to prioritze specific datasets on the fly, which allows the user to already start plotting before all data is populated.
  - **DataOperations**  
    DataOperations contain standard functions like resampling.
  - **TimeStamp**  
    TimeStamp contains a handy implementation of a timestamp object that can be used to deal with time.
  - **Exporters**  
    Exporters are used to generate files in different formats and export them to the system.

