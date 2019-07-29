from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums
gc = CyGlobalContext()

class TimeKeeper:

	def __init__(self):
		pass

	def interfaceScreen(self):
		screen = CyGInterfaceScreen("TimeKeeperScreen", CvScreenEnums.TIMEKEEPER)
		screen.addPanel( "MainBG", u"", u"", True, False, -10, -10, screen.getXResolution() + 20, screen.getYResolution() + 20, PanelStyles.PANEL_STYLE_MAIN )
		screen.showScreen(PopupStates.POPUPSTATE_IMMEDIATE, False)
		screen.setText("TimeKeeperExit", "Background", u"<font=4b>" + CyTranslator().getText("TXT_KEY_PEDIA_SCREEN_EXIT", ()).upper() + "</font>", CvUtil.FONT_RIGHT_JUSTIFY, screen.getXResolution() - 25, screen.getYResolution() - 40, -0.1, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_CLOSE_SCREEN, -1, -1 )

		iWidth = screen.getXResolution() - 40
		iHeight = (screen.getYResolution() - 100)/24 * 24 + 2
		iNumColumns = gc.getNumGameSpeedInfos()
		screen.addTableControlGFC("TimeKeeperTable", iNumColumns + 1, 20, 60, iWidth, iHeight, True, False, 24, 24, TableStyles.TABLE_STYLE_STANDARD )
		screen.setTableColumnHeader("TimeKeeperTable", 0, "", 120)
		iMaxIncrements = 0
		for i in xrange(gc.getNumGameSpeedInfos()):
			SpeedInfo = gc.getGameSpeedInfo(i)
			screen.setTableColumnHeader("TimeKeeperTable", i + 1, SpeedInfo.getDescription(), (iWidth - 120)/iNumColumns)
			iMaxIncrements = max(iMaxIncrements, SpeedInfo.getNumTurnIncrements())
		for i in xrange(iMaxIncrements * 5 + gc.getNumEraInfos() + 3):
			screen.appendTableRow("TimeKeeperTable")

		for i in xrange(gc.getNumGameSpeedInfos()):
			iStartYear = CyGame().getStartYear()
			SpeedInfo = gc.getGameSpeedInfo(i)
			iTotalTurns = 0
			iRow = 0
			for j in xrange(SpeedInfo.getNumTurnIncrements()):
				iTurns = SpeedInfo.getGameTurnInfo(j).iNumGameTurnsPerIncrement
				iIncrement = SpeedInfo.getDateIncrement(j).iIncrementDay
				iIncrement += SpeedInfo.getDateIncrement(j).iIncrementMonth * 30
				sIncrement = self.separateYearMonthDay(iIncrement)
				iDuration = iTurns * iIncrement
				sDuration = self.separateYearMonthDay(iDuration)
				screen.setTableText("TimeKeeperTable", 0, iRow, "<font=3>" + CyTranslator().getText("TXT_KEY_WB_START_YEAR", ()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				screen.setTableText("TimeKeeperTable", i + 1, iRow, "<font=3>" + CyGameTextMgr().getDateStr(iTotalTurns, False, CalendarTypes.CALENDAR_NO_SEASONS, iStartYear, i) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				iRow += 1
				screen.setTableText("TimeKeeperTable", 0, iRow, "<font=3>" + CyTranslator().getText("TXT_KEY_WB_TURNS", ()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				screen.setTableText("TimeKeeperTable", i + 1, iRow, "<font=3>" + str(iTurns) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				iRow += 1
				screen.setTableText("TimeKeeperTable", 0, iRow, "<font=3>" + CyTranslator().getText("TXT_KEY_INCREMENT", ()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				screen.setTableText("TimeKeeperTable", i + 1, iRow, "<font=3>" + sIncrement + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				iRow += 1
				screen.setTableText("TimeKeeperTable", 0, iRow, "<font=3>" + CyTranslator().getText("TXT_KEY_DURATION", ()) + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				screen.setTableText("TimeKeeperTable", i + 1, iRow, "<font=3>" + sDuration + "</font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				iRow += 2
				iTotalTurns += iTurns
			sColor = CyTranslator().getText("[COLOR_SELECTED_TEXT]", ())
			iRow = iMaxIncrements * 5
			screen.setTableText("TimeKeeperTable", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_END_YEAR", ()) + "</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.setTableText("TimeKeeperTable", i + 1, iRow, "<font=3>" + sColor + CyGameTextMgr().getDateStr(iTotalTurns, False, CalendarTypes.CALENDAR_NO_SEASONS, iStartYear, i) + "</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			iRow += 1
			screen.setTableText("TimeKeeperTable", 0, iRow, "<font=3>" + sColor + CyTranslator().getText("TXT_KEY_WB_TURNS", ()) + "</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			screen.setTableText("TimeKeeperTable", i + 1, iRow, "<font=3>" + sColor + str(iTotalTurns) + "</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
			iRow += 2

			sColor = CyTranslator().getText("[COLOR_UNIT_TEXT]", ())
			for k in xrange(gc.getNumEraInfos()):
				iStartTurn = gc.getEraInfo(k).getStartPercent() * iTotalTurns / 100
				screen.setTableText("TimeKeeperTable", 0, iRow, "<font=3>" + sColor + gc.getEraInfo(k).getDescription() + "</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				screen.setTableText("TimeKeeperTable", i + 1, iRow, "<font=3>" + sColor + CyGameTextMgr().getDateStr(iStartTurn, False, CalendarTypes.CALENDAR_NO_SEASONS, iStartYear, i) + "</color></font>", "", WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
				iRow += 1

	def separateYearMonthDay(self, iValue):
		iValueYear = iValue /360
		iValueMonth = (iValue % 360) /30
		iValueDay = (iValue % 360) % 30
		sValue = ""
		if iValueYear > 0:
			sValue += str(iValueYear) + " YR "
		if iValueMonth > 0:
			sValue += str(iValueMonth) + " MTH "
		if iValueDay > 0:
			sValue += str(iValueDay) + " DAY"
		return sValue

	def handleInput (self, inputClass):
		return 0

	def update(self, fDelta):
		return 1