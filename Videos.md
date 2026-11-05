# Videos
 In this document, you will find videos related to the RCS_AVR project along with their descriptions, which are divided into three categories: 
 - **Hardware design videos**
 - **Assembly and soldering videos**
 - **Coding and testing videos**

```plaintext
RCS, AVR Project
├── PCB
│   ├── [aKaReZa 70 - RCS, AVR - PART A]
│   │     ├─ Kickoff — Conceptual design, enclosure, Git/GitHub.
│   │     └─ Schematic — Altium Designer introduction and setup.
│   │
│   ├── [aKaReZa 71 - RCS, AVR - PART B]
│   │     ├─ Power Supply — 12V, 5V, 4V, 3.3V design.
│   │     └─ Safety — Fusing and voltage indicators.
│   │
│   ├── [aKaReZa 72 - RCS, AVR - PART C]
│   │     ├─ Collaboration — Issues, Pull Requests, Git tools.
│   │     └─ Power — Final regulator setup and LED resistors.
│   │
│   ├── [aKaReZa 73 - RCS, AVR - PART D]
│   │     ├─ Logic & Outputs — ATMega328, Reset, Relays, ULN2003.
│   │     ├─ Protection — Flywheel diode and LED indicators.
│   │     └─ Structure — Hierarchical design + library file export.
│   │
│   ├── [aKaReZa 75 - RCS, AVR - PART E]
│   │     ├─ LCD & Harness — Alphanumeric display and Harness tool.
│   │     ├─ GSM — SIMCard, UART protocol, filters, network LED.
│   │     └─ Review — GitHub Pull Requests and GSM tips.
│   │
│   ├── [aKaReZa 79 - RCS, AVR - PART F]
│   │     ├─ Input — 220V detection and signal handling.
│   │     ├─ Remote Control — Section creation and error fixes.
│   │     └─ Integration — Board shape design + Hamibash feature.
│   │
│   ├── [aKaReZa 81 - RCS, AVR - PART G]
│   │     ├─ Schematic Updates — GitHub issues and fixes.
│   │     ├─ PCB — Component placement and routing.
│   │     └─ Capacitors — Placement technique and performance tip.
│   │
│   └── [aKaReZa 83 - RCS, AVR - PART H]
│         ├─ Routing — Completion and optimization.
│         ├─ Tools — Polygon, Teardrops, Via Stitching.
│         └─ Mounting — Screw placement and final fixes.
│ 
├── Soldering
│   ├── [aKaReZa 100 - RCS, AVR - PART A]
│   │     ├─ Sections — 12V, 5V, 3.3V regulators.
│   │     ├─ MCU — ATMEGA soldering + relay + LCD.
│   │     └─ Testing — Segment-wise validation.
│   │
│   └── [aKaReZa 104 - RCS, AVR - PART B]
│         ├─ LCD Connector — Precision soldering tips.
│         ├─ Components — 4V reg, Quectel M66, SIM filters.
│         └─ Final Test — Complete module evaluation.
│ 
└── Programming
    ├── [aKaReZa 106 - RCS, AVR - PART I]
    │     ├─ PlatformIO — Project setup and structure.
    │     ├─ LCD — HomePage design and initialization.
    │     ├─ Relays — Output pin config and tests.
    │     └─ Control — Modular output handling logic.
    │
    ├── [aKaReZa 109 - RCS, AVR - PART J]
    │     ├─ Control — Output status display on LCD.
    │     ├─ GSM — Signal strength and registration via M66.
    │     ├─ Logic — State machine design.
    │     └─ Interface — Graphical LCD menu implementation.
    │
    ├── [aKaReZa 119 - RCS, AVR - PART K]
    │     ├─ Libraries — Project structure and GSM setup.
    │     ├─ SMS Rx — Parsing incoming messages.
    │     ├─ Format — Suggested SMS command syntax.
    │     ├─ Control — Output switching via SMS.
    │     ├─ Reply — Sending confirmation messages.
    │     ├─ Case Handling — `str_toUpper()` for flexibility.
    │     └─ Stability — Debugging and system reliability.
    │
    ├── [aKaReZa 125 - RCS, AVR - PART L]
    │     ├─ Git — Branch creation and version control.
    │     ├─ State Machine — System behavior management.
    │     ├─ Scheduler — Task configuration and execution.
    │     ├─ Tasks — Output and display routines.
    │     ├─ GSM States — One-time operations and transitions.
    │     └─ Deployment — Pushing changes to GitHub.
    │
    ├── [aKaReZa 128 - RCS, AVR - PART M]
    │   ├─ GSM_Reset — Definition and safe reinitialization flow.
    │   ├─ GSM_StartUp — Sequencing and non-blocking checks.
    │   ├─ LCD Feedback — Live state reflection on display.
    │   ├─ GSM_Init — Resource setup and readiness validation.
    │   └─ GSM_Idle — Event-driven loop and transition rules.
    │
    └── [aKaReZa 132 - RCS, AVR - PART N]
        ├─ Detection — Finding new SMS messages from GSM.
        ├─ GSM_SMS_ParseNumber — Extracting message index.
        ├─ GSM_SMS_ReadContent — Getting sender number and text.
        ├─ GSM_SMS_SendSMS — Reply SMS state machine.
        ├─ LCD — Busy icon, Welcome Page redesign, FW version.
        ├─ Code — Readability and structure improvements.
        └─ Git — Introduction to git merge workflow.
```

## PCB Desing Videos
<table style="border-collapse: collapse;">
  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/KLH8E3yOOMw">aKaReZa 70 – RCS, AVR - PART A</a>
      </h3>
      <p style="margin: 8px 0 0;">
        This first video in the series covers designing a project from idea to final product, including conceptual design, casing selection, schematic creation in Altium Designer, and more.
      </p>
    </td>
    <td width="360" valign="top" style="padding: 0;">
      <a href="https://youtu.be/KLH8E3yOOMw">
        <img src="https://img.youtube.com/vi/KLH8E3yOOMw/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 70 – RCS, AVR - PART A Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/-EyT4b3-hQU">aKaReZa 71 – RCS, AVR - PART B</a>
      </h3>
      <p style="margin: 8px 0 0;">
        In this video, we design a 12-5-4-3.3V power supply, add fuses, and include voltage indicators.
      </p>
    </td>
    <td width="360" valign="top" style="padding: 0;">
      <a href="https://youtu.be/-EyT4b3-hQU">
        <img src="https://img.youtube.com/vi/-EyT4b3-hQU/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 71 – RCS, AVR - PART B Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/bh53Z6KH728">aKaReZa 72 – RCS, AVR - PART C</a>
      </h3>
      <p style="margin: 8px 0 0;">
        In this video, we review all your Issues and Pull Requests, introduce some useful software, complete the project's power supply section, and discuss upcoming channel content and plans.
      </p>
    </td>
    <td width="360" valign="top" style="padding: 0;">
      <a href="https://youtu.be/bh53Z6KH728">
        <img src="https://img.youtube.com/vi/bh53Z6KH728/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 72 – RCS, AVR - PART C Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/vWiSh-8D4kY">aKaReZa 73 – RCS, AVR - PART D</a>
      </h3>
      <p style="margin: 8px 0 0;">
        In this video, after designing the controller and output sections, we learn hierarchical design, create the project hierarchically, and finally generate the project's library file.
      </p>
    </td>
    <td width="360" valign="top" style="padding: 0;">
      <a href="https://youtu.be/vWiSh-8D4kY">
        <img src="https://img.youtube.com/vi/vWiSh-8D4kY/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 73 – RCS, AVR - PART D Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/V0r4XVZA-bk">aKaReZa 75 – RCS, AVR - PART E</a>
      </h3>
      <p style="margin: 8px 0 0;">
        In this video, we cover using character LCDs, the Harness tool in Altium Designer, identifying VCC, VEE, VDD, and VSS pins, connecting a SIM card and UART protocol to a GSM module, adding power filters and a network status LED to the GSM module, and reviewing some GSM module tips. Additionally, we check the raised Pull Requests and Issues.
      </p>
    </td>
    <td width="360" valign="top" style="padding: 0;">
      <a href="https://youtu.be/V0r4XVZA-bk">
        <img src="https://img.youtube.com/vi/V0r4XVZA-bk/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 75 – RCS, AVR - PART E Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/U1JYVCcn27k">aKaReZa 79 – RCS, AVR - PART F</a>
      </h3>
      <p style="margin: 8px 0 0;">
        In this video, we review raised Pull Requests and Issues, organize the project, design the input section and 220V signal detection, design the code-learn remote section, fix errors, resize the board to fit the casing, introduce the "Hami Bash" section, and discuss the idea of casing installation.
      </p>
    </td>
    <td width="360" valign="top" style="padding: 0;">
      <a href="https://youtu.be/U1JYVCcn27k">
        <img src="https://img.youtube.com/vi/U1JYVCcn27k/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 79 – RCS, AVR - PART F Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/6VrP90s_HFc">aKaReZa 81 – RCS, AVR - PART G</a>
      </h3>
      <p style="margin: 8px 0 0;">
        In this video, we address the issues raised on GitHub, make several schematic changes, place components in the PCB environment, route several sections, and learn a crucial tip about capacitor placement.
      </p>
    </td>
    <td width="360" valign="top" style="padding: 0;">
      <a href="https://youtu.be/6VrP90s_HFc">
        <img src="https://img.youtube.com/vi/6VrP90s_HFc/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 81 – RCS, AVR - PART G Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/ByZ2kSmf6VU">aKaReZa 83 – RCS, AVR - PART H</a>
      </h3>
      <p style="margin: 8px 0 0;">
        In this video, we continue routing the board. After completing the routes, we learn how to strengthen power paths, place screws on the board, and use the Polygon, Teardrops, and Via stitching tools.
      </p>
    </td>
    <td width="360" valign="top" style="padding: 0;">
      <a href="https://youtu.be/ByZ2kSmf6VU">
        <img src="https://img.youtube.com/vi/ByZ2kSmf6VU/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 83 – RCS, AVR - PART H Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/FoJUFKjwWjk">aKaReZa 98 – PCB, Ordering</a>
      </h3>
      <p style="margin: 8px 0 0;">
        In this video, explore essential steps for ordering PCB, from fixing issues before printing, reviewing design rules, error troubleshooting, professional documentation, and learning about local companies and Cheap Project group for seamless PCB manufacturing.
      </p>
    </td>
    <td width="360" valign="top">
      <a href="https://youtu.be/FoJUFKjwWjk">
        <img src="https://img.youtube.com/vi/FoJUFKjwWjk/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 98 – PCB, Ordering Thumbnail"/>
      </a>
    </td>
  </tr>  
</table>

## Assembly and soldering videos
<table style="border-collapse: collapse;">
  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/UuL0Z89b1r0">aKaReZa 100 – Repair, RCS, AVR - PART A</a>
      </h3>
      <p style="margin: 8px 0 0;">
        Learn the full soldering and assembly process of the RCS-AVR board. This video walks you through every step, including soldering power inputs, voltage regulators (12V, 5V, 3.3V), microcontroller placement, relay and LCD installation, and performing circuit tests after each phase to ensure proper functionality. A must-watch for anyone wanting practical experience in microcontroller board repair and troubleshooting.
      </p>
    </td>
    <td width="360" valign="top">
      <a href="https://youtu.be/UuL0Z89b1r0">
        <img src="https://img.youtube.com/vi/UuL0Z89b1r0/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 100 – Repair, RCS, AVR - PART A Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/i9ef0sJyjoc">aKaReZa 104 – Repair, RCS AVR Soldering - PART B</a>
      </h3>
      <p style="margin: 8px 0 0;">
        Continue soldering the RCS-AVR project with a focus on communication modules. This video covers LCD connector soldering, 4V regulator installation, Quectel M66 module assembly, SIM card connector setup, remaining component soldering, and final system testing to ensure everything functions correctly. Perfect for microcontroller board assembly and troubleshooting enthusiasts.
      </p>
    </td>
    <td width="360" valign="top">
      <a href="https://youtu.be/i9ef0sJyjoc">
        <img src="https://img.youtube.com/vi/i9ef0sJyjoc/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 104 – Repair, RCS AVR Soldering - PART B Thumbnail"/>
      </a>
    </td>
  </tr> 
</table>

## Coding and testing videos
<table style="border-collapse: collapse;">
  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/397Jk0z9aTY">aKaReZa 106 – RCS, AVR - PART I</a>
      </h3>
      <p style="margin: 8px 0 0;">
        Start coding the RCS-AVR project using PlatformIO, with a focus on LCD initialization, homepage design, relay control, and hardware interaction. This video lays the groundwork for structured, modular AVR programming tailored to practical embedded systems development.
      </p>
    </td>
    <td width="360" valign="top">
      <a href="https://youtu.be/397Jk0z9aTY">
        <img src="https://img.youtube.com/vi/397Jk0z9aTY/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 106 – RCS, AVR - PART I Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/tSlz7DpWGOs">aKaReZa 109 – RCS, AVR - PART J</a>
      </h3>
      <p style="margin: 8px 0 0;">
        Advance the RCS-AVR project by implementing output control, reading signal strength, managing network registration, and designing LCD menus. This video explores state machines, real-time status display, and software-hardware interaction—ideal for building smart embedded systems.
      </p>
    </td>
    <td width="360" valign="top">
      <a href="https://youtu.be/tSlz7DpWGOs">
        <img src="https://img.youtube.com/vi/tSlz7DpWGOs/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 109 – RCS, AVR - PART J Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/OboAPzh3QSU">aKaReZa 119 – RCS, AVR - PART K</a>
      </h3>
      <p style="margin: 8px 0 0;">
        Build SMS-based control systems with AVR and the Quectel M66 module. This video covers message parsing, output control, reply generation, and case-insensitive command handling using <code>str_toUpper()</code>—a practical guide for robust GSM integration in embedded projects.
      </p>
    </td>
    <td width="360" valign="top">
      <a href="https://youtu.be/OboAPzh3QSU">
        <img src="https://img.youtube.com/vi/OboAPzh3QSU/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 119 – RCS, AVR - PART K Thumbnail"/>
      </a>
    </td>

  <tr>
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/RPZP1AIJVvo">aKaReZa 125 – RCS, AVR - PART L</a>
      </h3>
      <p style="margin: 8px 0 0;">
        Advance your embedded software development workflow with Git and structured logic. This episode introduces Git branching for safe feature development, implements a robust State Machine, configures a Scheduler, and defines tasks for output and display control. It also reviews GSM module states and one-time operations, culminating in a clean GitHub push—ideal for building scalable, maintainable AVR-based systems.
      </p>
    </td>
    <td width="360" valign="top">
      <a href="https://youtu.be/RPZP1AIJVvo">
        <img src="https://img.youtube.com/vi/RPZP1AIJVvo/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 125 – RCS, AVR - PART L Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr style="background-color: #f9f9f9;">
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/RqF51kzmc10">aKaReZa 128 – RCS, AVR - PART M</a>
      </h3>
      <p style="margin: 8px 0 0;">
        Deepen your understanding of <strong>State Machines in embedded systems</strong> with this AVR-based RCS project episode. You’ll learn how to implement and manage <strong>GSM_Reset, GSM_StartUp, GSM_Init, and GSM_Idle states</strong>, while reflecting transitions on the LCD. The tutorial emphasizes <strong>non-blocking code design principles</strong>, making it a crucial step toward building smarter, more reliable GSM-driven control systems.
      </p>
    </td>
    <td width="360" valign="top">
      <a href="https://youtu.be/RqF51kzmc10">
        <img src="https://img.youtube.com/vi/RqF51kzmc10/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 128 – RCS, AVR - PART N Thumbnail"/>
      </a>
    </td>
  </tr>

  <tr style="background-color: #f9f9f9;">
    <td valign="top" style="padding: 0 10px;">
      <h3 style="margin: 0;">
        <a href="https://youtu.be/P0FgmPWP3M4">aKaReZa 132 – RCS, AVR - PART N</a>
      </h3>
      <p style="margin: 8px 0 0;">
        Implement fully non‑blocking <strong>SMS receive/send state machines</strong> in the RCS‑AVR project. This episode walks through detecting new messages, parsing the SMS index, extracting the sender’s number and text, and sending replies through the GSM module. You’ll also enhance the LCD interface with <strong>custom characters</strong>, redesign the <strong>Welcome Page</strong>, display the firmware version, and learn how to integrate changes using <strong>git merge</strong> for a cleaner workflow.
      </p>
    </td>
    <td width="360" valign="middle">
      <a href="https://youtu.be/P0FgmPWP3M4">
        <img src="https://img.youtube.com/vi/P0FgmPWP3M4/maxresdefault.jpg"
             width="360"
             alt="aKaReZa 132 – RCS, AVR - PART O Thumbnail"/>
      </a>
    </td>
  </tr>
  
</table>


# 🌟 Support Me
If you found this repository useful:
- Subscribe to my [YouTube Channel](https://www.youtube.com/@aKaReZa75).
- Share this repository with others.
- Give this repository and my other repositories a star.
- Follow my [GitHub account](https://github.com/aKaReZa75).

# ✉️ Contact Me
Feel free to reach out to me through any of the following platforms:
- 📧 [Email: aKaReZa75@gmail.com](mailto:aKaReZa75@gmail.com)
- 🎥 [YouTube: @aKaReZa75](https://www.youtube.com/@aKaReZa75)
- 💼 [LinkedIn: @akareza75](https://www.linkedin.com/in/akareza75)
