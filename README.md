Changes in the last version Astrolog 5.41G

   * Fixed original Astrolog bug, where Local Horizon data (and also Prime
     Vertical) were off by up to several arc-minutes.

   * Fixed original Astrolog bug (Windows version), where "Time / Space
     Midpoint" chart is correct, but it was not possible to return back to
     "No relationship chart" - there was still previous "midpoint" chart.

   * In "Rising and Setting" chart fixed bug, where "Print Nearest Second"
     worked unproperly - seconds weren't shown for risings and settings.

     In this context more correct termin instead of "zenith" and "nadir" is
     astronomical termin "culmination" that means transit over meridian.
     There are two culminations: "upper culmination" when planet has
     highest position and "lower culmination" when planet has lowest
     position. So in the chart termins "zeniths" and "nadirs" are replaced
     by "culm.(up)" and "culm.(lo)" respectively.

   * Uncorrect object name "Nadir" has been replaced by corret "IC". Nadir
     has different meaning than IC, Nadir is the point on the sky opposite
     to Zenith.

   * Slightly improved Astrolog's behaviour in case of progressed and
     relationship charts. Earlyer Windows versions allowed to do
     progression and then do some relationship chart like Natal-Progressed.
     Or vice versa, when looking say Natal-Progressed chart switch also
     Progression "on". That all confused Astrolog and results were often
     unpredictable.

     Now any use of relationship charts (in menu "Info") switches off "Do
     Progression" (in "Chart" => "Progressions"). And vice versa, switching
     progression on (in "Chart" => "Progression") switches off any
     comparison. As result Astrolog is displaying just what it has to
     dasplay, no confusion.

   * Fixed orignal astrolog bug (appeared only in Windows version). When
     natal chart was opened from file with saved planets positions (not
     birth data), there only first output was correct, all next charts (as
     aspect list, transit searches, all comparison charts etc) had wrong
     planets positions.

     NB! Use such input files carefully! They are treated by astrolog as
     files "with no time and space", so a lot of chart types can't be used
     with them. An examples are all progression charts, because they needs
     birth-chart time to calculate progressions.

   * Fixed bug of changed versions, where in case of use of data with saved
     planet positions (just as above), data-border of graphics charts
     dilsplayed info "no time or space" twice.

   * Time and location have new format - with seconds, both for chart's
     data inputs and outputs. Only outputs with old data format are events
     searching outputs, because with reasonably low division value they
     anyway can be off by a few minutes, so seconds seems unnecessary
     there.

          "Rising and setting" chart is only exception among searching
          charts that shows time with seconds. Even with default setting d:
          48 times are off by only a few seconds, and it is quick enough to
          show precise (by seconds) times with d: 96 in reasonably short
          time. Note, that values of azimuth angles in rising/setting
          moment are extremely sensitive of time changes (very small change
          of time causes big change in azimuth) and can be off by a few
          seconds. To improve azimuth precision one has to increase
          division value up to hundreds.

     All input files with old time/location format can be used, but all
     outputs are saved only in new time/location format.

   * Calculations precision improved - time-dependent obliquity of ecliptic
     is used instead of fixed value.

   * Added new switsh to change local horizon text output from default ENWS
     to NESW. One has just to add line

          =YZ

     to astrolog.dat file and it will be deafult setting.

   * Slightly changed Prime Vertical text chart output: Altitude and
     Azimuth in header have been replaced by Amplitude and Prm-Vrt. One can
     see this chart by choosing "Local Horizon" and setting "Horizon Chart
     with Polar Center / Prime Vert. (text)" in "Chart settings". Note,
     that this switch has different meanings for text and graphics charts:
     in text mode it switches between Local Horizon <=> Prime Vertical
     outputs, in graphics it switches between "normal wiew" and "view vith
     Polar Center". However, one can switch also between Polar Center and
     Pime Vert. also in graphics by hitting 'i' on the keyboard (switch to
     bonus mode).

   * Changed calculations of Solar Arc. Original Astrolog calculates there
     actually directions, where all planets and house cusps positions are
     moved forward to an amount equal in degrees to the number of years
     that have passed between the specified date and the chart in question.
     Because real (even mean or average) motion of Sun isn't 1 degree per
     day, resulting Solar Arc Sun position doesn't match with secondary
     progressed Sun position (but they must match).

     To correct this situation, there has been added another calculation of
     Solar Arc - first secondary progressed Sun's position is determined
     and then all chart components are moved accordlingly.

     To avoid misunderstanding, existing Solar Acr calculations remains,
     but have now corrected name "Degree Per Day/Month", as earlyer one can
     change amount of degrees per year (more strictly, amount of days for 1
     degree direction. See description of -p0 and -pd switches in
     helpfile.540). Added calculation is named "Solar Arc Directions" and
     there Sun's position follows secondary progressed Sun positions, all
     other point in chart are moved accordlingly.

     Existing switches -p0, -p0n remain as previously. For new correct
     "Solar Arc Directions" calculations new switches -p1 and -p1n are
     used. In Windows version all can be done through menus: "Chart" =>
     "Progressions..."

   * Slightly changed chraphics charts' infoborder and text charts' headers
     - secondary progressed charts are still named as "progressed", Solar
     Arc and "Degree per Year/Mont" are marked as "directed" with name of
     direction.

   * Graphics chart's infoborder has additional information: Obliquity of
     ecliptic, Sidereal time, Delta T (in seconds) and in case of sidereal
     chart Ayanamsha (sidereal offest).

     Note, that Ayanamsha has negative value and dafaults to Fagan Bradley.
     Ayanamsha control in "Calculation Settings" has a dropdown to allow
     quick selection of some common systems of sidereal astrology. The
     values are additions to default value and they are 0.0 for Fagan
     Bradley, 0.883333 (or 0 degrees 53') for N.C. Lahiri, 0.983333 (or 0
     degrees 59') for Krishnamurti, and 2.333333 (or 2 degrees 20') for
     B.V. Raman. On the screen has shown resulting value.

   * Date/time and Julian Day in the graphics charts' infoboredr has been
     colored:

          1. First (or single) data/time is always bright white.

          2. Second date/time is always yellow.

          3. JD (which is actually also time/date) has color of
          corresponding date/time above (as yellow in case of transit
          comparison or direction charts). If JD doesn't correspond to
          neither date/time above (as in case of progressions where JD
          corresponds to date/time of planets' positions on the screen),
          color is green. Note, that in case of Synastry and Comparison
          charts JD will be yellow, becsuse it corresponds to second
          date/time above (data of second chart).

   * Windows version controls of progressed charts have been polished. They
     looks more clear now. As default 365.2422 (tropical) year has used.

     In "Degree per Days" dropdown are also available:

          365.25636 (sidereal year)
          27.321582 (tropical month)
          27.321661 (sidereal month)
          29.530588 (synodic month)

     Other more exotic (like Draconic) years/months aren't included.

   * Astrolog computes position of Lilith (Dark Moon) using external
     ephemeris. When ephemeris are set off, Astrolog will display the
     position of the South Node instead (see helpfile.540, description of
     -HO switch). For users, who wants to use South Node always, there has
     been added new switch =YN which forces Astrolog to do it. This switch
     can be entered through "Edit" => "Enter Command Line" (Windows
     version) or simply added to astrolog.dat file to make this behaviour
     default.

   * By default Astrolog calculates mean Lilith. For users who wants to use
     osculting position of Lilith, new switch -YL has been added. As above,
     this switch can be added to astrolog.dat file, which makes such
     behaviour default.

   * Dispositors glyphs on the graphics wheels can be switched on/off
     using switch -YD. It can be added to astrolg.dat file: =YD (on) or
     _YD (off), default "on".

---------------------------------------------------------------------------

18th. May. 2002



