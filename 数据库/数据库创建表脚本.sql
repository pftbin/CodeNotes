-- MySQL dump 10.13  Distrib 5.6.17, for Win64 (x86_64)
--
-- Host: 172.16.168.205    Database: studiodb
-- ------------------------------------------------------
-- Server version	5.6.28-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `sbt_airuserinfo`
--

DROP TABLE IF EXISTS `sbt_airuserinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_airuserinfo` (
  `USERID` int(10) NOT NULL,
  `ROLEID` int(10) DEFAULT NULL,
  `LOGINNAME` varchar(32) COLLATE utf8mb4_unicode_ci NOT NULL,
  `USERNAME` varchar(32) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `LOGINPASSWORD` varchar(64) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `OFFICETELNO` varchar(32) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `HOMETELNO` varchar(32) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `EMAILADDRESS` varchar(64) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `USEDSPACE` int(10) DEFAULT NULL,
  `MAXSPACE` int(10) DEFAULT NULL,
  `POSTID` varchar(6) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `DEPARTMENTID` int(10) DEFAULT NULL,
  `SBMMUSERID` int(10) DEFAULT NULL,
  `SBMMUSERIDMANUL` int(10) DEFAULT NULL,
  `SITE` varchar(64) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `USERCODE` varchar(64) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`USERID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_applyex`
--

DROP TABLE IF EXISTS `sbt_applyex`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_applyex` (
  `APPLYID` int(10) DEFAULT NULL,
  `ITYPE` int(10) DEFAULT NULL,
  `STARTTIME` date DEFAULT NULL,
  `ENDTIME` date DEFAULT NULL,
  `STRPLAYLIST` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ASSOCID` int(10) DEFAULT NULL,
  `USEID` int(10) DEFAULT NULL,
  `STRPARAM` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `PGMCODE` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NCULOMNID` int(10) DEFAULT NULL,
  `NBANCIID` int(10) DEFAULT '0',
  `PARENTID` int(10) NOT NULL DEFAULT '0',
  `FIRSTPLAYDAY` date DEFAULT NULL,
  `CLIPDEST` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `RECORDNO` int(10) DEFAULT '-1',
  `NSTATE` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_basedatainfo`
--

DROP TABLE IF EXISTS `sbt_basedatainfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_basedatainfo` (
  `DATANAME` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `DATAVALUE` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `DATATYPE` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`DATANAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_channelgroup`
--

DROP TABLE IF EXISTS `sbt_channelgroup`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_channelgroup` (
  `GROUPID` int(10) NOT NULL,
  `GROUPNAME` varchar(128) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `SHOWNO` int(10) DEFAULT NULL,
  `CHANNELTYPE` int(10) DEFAULT NULL,
  `STUDIOID` int(10) NOT NULL,
  `ASSIGNTYPE` int(10) NOT NULL DEFAULT '0',
  `CREATETYPE` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`GROUPID`),
  UNIQUE KEY `U_SHOWNO` (`STUDIOID`,`SHOWNO`),
  UNIQUE KEY `U_GROUPNAME` (`GROUPNAME`,`STUDIOID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_chlctrl`
--

DROP TABLE IF EXISTS `sbt_chlctrl`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_chlctrl` (
  `CHANNELID` int(10) DEFAULT NULL,
  `GROUPID` int(10) DEFAULT NULL,
  `APPLYID` varchar(10) COLLATE utf8mb4_unicode_ci DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_columninfo`
--

DROP TABLE IF EXISTS `sbt_columninfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_columninfo` (
  `NITEMID` int(10) NOT NULL,
  `NITEMTYPE` int(10) NOT NULL,
  `STRITEMNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRITEMCODE` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NBELONGID` int(10) NOT NULL DEFAULT '0',
  `SITE` varchar(64) COLLATE utf8mb4_unicode_ci DEFAULT '0',
  PRIMARY KEY (`NITEMID`,`NITEMTYPE`),
  KEY `N_STRCOLUMNNAME` (`STRITEMNAME`(191))
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_controlchannel`
--

DROP TABLE IF EXISTS `sbt_controlchannel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_controlchannel` (
  `CHANNELID` int(10) NOT NULL,
  `CONTROLID` int(10) NOT NULL,
  `GROUPID` int(10) NOT NULL,
  `CONTRLCOM` int(10) DEFAULT NULL,
  `VTRID` int(10) DEFAULT NULL,
  `VTRCOM` int(10) DEFAULT NULL,
  `CTRLEDPORT` int(10) DEFAULT NULL,
  `CTRLTYPE` int(10) DEFAULT NULL,
  `CTRLPORT` int(10) DEFAULT NULL,
  `CTRLEDPROTOCOL` int(10) DEFAULT NULL,
  PRIMARY KEY (`CHANNELID`,`CONTROLID`,`GROUPID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_controlparam`
--

DROP TABLE IF EXISTS `sbt_controlparam`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_controlparam` (
  `CONTROLID` int(10) NOT NULL,
  `PCIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `PCNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `GPICOM` int(10) DEFAULT NULL,
  `TALLYCOM` int(10) DEFAULT NULL,
  `OUTSHOWCOM` int(10) DEFAULT NULL,
  `CTRLCOM` int(10) DEFAULT NULL,
  `SWITCHCOM` int(10) DEFAULT NULL,
  `CHANNELINT` int(10) DEFAULT NULL,
  `BITMAPROOT` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `CANPAUSE` int(10) DEFAULT NULL,
  `PERMITIPS` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `FORBIDIPS` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `CMDDELAYTIME` int(10) DEFAULT NULL,
  `FRAMETIMEOUT` int(10) DEFAULT NULL,
  `OUTSTOPTYPE` int(10) DEFAULT NULL,
  `SYNDELAYTIME` int(10) DEFAULT NULL,
  `WORKTYE` int(10) DEFAULT NULL,
  `WORKSTATUS` int(10) DEFAULT NULL,
  `USERID` int(10) DEFAULT NULL,
  `HOSTCONTROLID` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`CONTROLID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_controlserver`
--

DROP TABLE IF EXISTS `sbt_controlserver`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_controlserver` (
  `CONTROLID` int(10) NOT NULL,
  `PCIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `PORT` int(10) DEFAULT NULL,
  `PCNAME` varchar(128) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `GPICOM` int(10) DEFAULT NULL,
  `TALLYCOM` int(10) DEFAULT NULL,
  `OUTSHOWCOM` int(10) DEFAULT NULL,
  `CTRLCOM` int(10) DEFAULT NULL,
  `SWITCHCOM` int(10) DEFAULT NULL,
  `SYNDELAYTIME` int(10) DEFAULT NULL,
  `WORKTYE` int(10) DEFAULT NULL,
  `WORKSTATUS` int(10) DEFAULT NULL,
  `HOSTCONTROLID` int(10) NOT NULL DEFAULT '0',
  `STUDIOID` int(10) DEFAULT NULL,
  `SERVERID` int(10) DEFAULT NULL,
  `CTRLIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`CONTROLID`),
  UNIQUE KEY `U_PCNAME` (`PCNAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_deltask`
--

DROP TABLE IF EXISTS `sbt_deltask`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_deltask` (
  `TASKID` int(10) NOT NULL,
  `MATERIALID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `LOCALFILE` varchar(128) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `GET1` int(10) DEFAULT '0',
  `PRIORITY` int(10) DEFAULT NULL,
  `TYPE` int(10) DEFAULT NULL,
  `SERVERID` int(10) DEFAULT NULL,
  PRIMARY KEY (`TASKID`),
  UNIQUE KEY `U_DELTASK` (`LOCALFILE`,`SERVERID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_eventinfoex`
--

DROP TABLE IF EXISTS `sbt_eventinfoex`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_eventinfoex` (
  `ID` int(10) NOT NULL,
  `STREVENTID` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `NEVENTNO` int(10) DEFAULT NULL,
  `NPLAYTYPE` int(10) DEFAULT NULL,
  `NSTATUS` int(10) DEFAULT NULL,
  `NSTORYIN` int(10) DEFAULT NULL,
  `NCHANNEL` int(10) DEFAULT NULL,
  `STREVENTNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MOSROID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NVERSION` int(10) DEFAULT NULL,
  `NEVENTTYPE` int(10) DEFAULT NULL,
  `STRLASTWORDS` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRDSCPT` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NLENGTH` int(10) DEFAULT NULL,
  `TMCREATETIME` date NOT NULL,
  `NREFCOUNTER` int(10) DEFAULT NULL,
  `BEDELETE` int(10) NOT NULL DEFAULT '0',
  `PREMARJIN` int(10) NOT NULL DEFAULT '0',
  `POSTMARJIN` int(10) NOT NULL DEFAULT '0',
  `PLAYTIME` date DEFAULT NULL,
  `STRITEMTRIGGER` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `OBJECTID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRSTORYID` varchar(512) COLLATE utf8mb4_unicode_ci NOT NULL,
  `NSOM` int(10) DEFAULT NULL,
  `NEOM` int(10) DEFAULT NULL,
  `NVOICELENGTH` int(10) DEFAULT NULL,
  `CUEPOINT` int(10) DEFAULT NULL,
  `KEYFRAMPOINT` int(10) DEFAULT NULL,
  `STRCHECKMAN` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NEVENTVERSION` int(10) DEFAULT NULL,
  `STROBJECTID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `VOLUMEALLOW` int(11) DEFAULT NULL,
  PRIMARY KEY (`STREVENTID`),
  UNIQUE KEY `U_NEVENTID` (`ID`),
  KEY `N_STROBJECT` (`STROBJECTID`(191)),
  KEY `N_STRSOTRYID` (`STRSTORYID`(191))
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_interface`
--

DROP TABLE IF EXISTS `sbt_interface`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_interface` (
  `SERVERID` int(10) NOT NULL,
  `SERVERIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `SERVERNAME` varchar(128) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MESSAGEBACKIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MESSAGEBACKMQNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `SERVERPORT` int(10) DEFAULT NULL,
  `MQNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `GENTASK` int(10) DEFAULT NULL,
  `MPCIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ADJUSTPORT` int(10) DEFAULT NULL,
  `SLAVESERVERIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`SERVERID`),
  UNIQUE KEY `U_INTERFACENAME` (`SERVERNAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_itemsplayed`
--

DROP TABLE IF EXISTS `sbt_itemsplayed`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_itemsplayed` (
  `ID` int(11) DEFAULT NULL,
  `EVENTID` int(11) NOT NULL,
  `EVENTNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NSTUDIOID` int(11) DEFAULT NULL,
  `NCOLUMNID` int(11) DEFAULT NULL,
  `PLAYLISTID` int(11) DEFAULT NULL,
  `STIMESTART` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `STIMEEND` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TIMESTART` date DEFAULT NULL,
  PRIMARY KEY (`EVENTID`,`STIMESTART`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_itemsplayeda`
--

DROP TABLE IF EXISTS `sbt_itemsplayeda`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_itemsplayeda` (
  `ID` int(11) DEFAULT NULL,
  `EVENTID` int(11) NOT NULL,
  `EVENTNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NSTUDIOID` int(11) DEFAULT NULL,
  `NCOLUMNID` int(11) DEFAULT NULL,
  `PLAYLISTID` int(11) DEFAULT NULL,
  `STIMESTART` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `STIMEEND` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TIMESTART` date DEFAULT NULL,
  PRIMARY KEY (`EVENTID`,`STIMESTART`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_lockstudio`
--

DROP TABLE IF EXISTS `sbt_lockstudio`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_lockstudio` (
  `APPLYID` int(10) NOT NULL,
  `STUDIOID` int(10) NOT NULL,
  `STATUS` int(10) NOT NULL,
  `STARTTIME` date DEFAULT NULL,
  `ENDTIME` date DEFAULT NULL,
  `TEMP1` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TEMP2` varchar(255) COLLATE utf8mb4_unicode_ci DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_materialinfo`
--

DROP TABLE IF EXISTS `sbt_materialinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_materialinfo` (
  `ID` int(10) NOT NULL,
  `STRMATERIALID` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `TMCREATETIME` date NOT NULL,
  `NSTATUS` int(10) DEFAULT NULL,
  `NSOF` int(10) DEFAULT NULL,
  `NEOF` int(10) DEFAULT NULL,
  `NLENGTH` int(10) DEFAULT NULL,
  `NREFCOUNTER` int(10) DEFAULT NULL,
  `NPLAYFREQ` int(10) DEFAULT NULL,
  `NPROTECTED` int(10) DEFAULT NULL,
  `STRLOCALFILE` varchar(512) COLLATE utf8mb4_unicode_ci NOT NULL,
  `NLOCALVERSION` int(10) DEFAULT NULL,
  `STRLOCALPATH` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRSOURCEID` varchar(40) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `CHECKSTATUS` int(10) DEFAULT NULL,
  `GRAPHID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TYPE` int(10) NOT NULL DEFAULT '0',
  `MOSITEMID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MOSITEMNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ISBLANK` int(10) NOT NULL DEFAULT '0',
  `STRNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRCOLUMN` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRSTORYMOSID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STREVENTID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`STRMATERIALID`),
  KEY `N_STRMATERIALSOURCEID` (`STRSOURCEID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_materialinstudio`
--

DROP TABLE IF EXISTS `sbt_materialinstudio`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_materialinstudio` (
  `ID` int(10) DEFAULT NULL,
  `NSTUDIOID` int(10) NOT NULL,
  `STRMATERIALID` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `TMCREATETIME` date DEFAULT NULL,
  `NSTATUS` int(10) DEFAULT NULL,
  `NPROTECTED` int(10) DEFAULT NULL,
  `NLOCALVERSION` int(10) NOT NULL DEFAULT '0',
  `STRLOCALPATH` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `CHECKSTATUS` int(11) DEFAULT NULL,
  `NSOM` int(10) DEFAULT NULL,
  `NEOM` int(10) DEFAULT NULL,
  `NLENGTH` int(10) DEFAULT NULL,
  PRIMARY KEY (`STRMATERIALID`,`NSTUDIOID`,`NLOCALVERSION`),
  KEY `N_NMATERIALSTUDIOID` (`NSTUDIOID`),
  KEY `N_STRMATERIALID` (`STRMATERIALID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_modifyhistory`
--

DROP TABLE IF EXISTS `sbt_modifyhistory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_modifyhistory` (
  `STROBJECTID` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `NATTRIBUTE` int(10) NOT NULL,
  `NTYPE` int(10) DEFAULT NULL,
  `STROPERATOR` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `USERID` int(10) NOT NULL,
  `TMACTTIME` date DEFAULT NULL,
  `PCIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `BROADMATERIALID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ARCHIVEMATERAILID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ISARCHIVED` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`STROBJECTID`,`NATTRIBUTE`),
  KEY `IDX_MODIFYHISTORY` (`STROBJECTID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_mschannel`
--

DROP TABLE IF EXISTS `sbt_mschannel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_mschannel` (
  `CHANNELID` int(10) NOT NULL,
  `SUBCHANNELID` int(10) NOT NULL,
  `SUBCONTRLCOM` int(10) NOT NULL,
  PRIMARY KEY (`CHANNELID`,`SUBCHANNELID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_nousestamp`
--

DROP TABLE IF EXISTS `sbt_nousestamp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_nousestamp` (
  `STRMATERIALID` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `NSTUDIO` int(11) NOT NULL,
  `CTRLIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`STRMATERIALID`,`NSTUDIO`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_playchannel`
--

DROP TABLE IF EXISTS `sbt_playchannel`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_playchannel` (
  `CHANNELID` int(10) NOT NULL,
  `CHANNELNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NPORT` int(10) DEFAULT NULL,
  `WORKTYPE` int(10) DEFAULT NULL,
  `CHANNELPARAM` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `SERVERID` int(10) DEFAULT NULL,
  `SWITCHIN` int(10) NOT NULL DEFAULT '-1',
  `SWITCHOUT` int(10) NOT NULL DEFAULT '-1',
  `WAITFRAME` int(10) NOT NULL DEFAULT '0',
  `PLID` int(10) NOT NULL DEFAULT '0',
  `COMPILEFRAME` int(10) NOT NULL DEFAULT '0',
  `CTRLIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NDATAPORT` int(11) DEFAULT '0',
  PRIMARY KEY (`CHANNELID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_playhistory`
--

DROP TABLE IF EXISTS `sbt_playhistory`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_playhistory` (
  `OBJECTID` varchar(512) COLLATE utf8mb4_unicode_ci NOT NULL,
  `NAME` varchar(512) COLLATE utf8mb4_unicode_ci NOT NULL,
  `TYPE` int(10) DEFAULT NULL,
  `BELONGID` varchar(512) COLLATE utf8mb4_unicode_ci NOT NULL,
  `PLAYBEGIN` date DEFAULT NULL,
  `PLAYEND` date DEFAULT NULL,
  `SOT` int(10) DEFAULT NULL,
  `EOT` int(10) DEFAULT NULL,
  `LENGTH` int(10) DEFAULT NULL,
  KEY `IDX_SBT_PLAYHISTORY` (`OBJECTID`(191),`TYPE`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_playlistex`
--

DROP TABLE IF EXISTS `sbt_playlistex`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_playlistex` (
  `ID` int(10) NOT NULL,
  `STRLISTID` varchar(512) COLLATE utf8mb4_unicode_ci NOT NULL,
  `STRLISTNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TMFIRSTPLAYDAY` date NOT NULL,
  `TMLASTPLAYDAY` date DEFAULT NULL,
  `TMCREATETIME` date DEFAULT NULL,
  `STREDITOR` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TMLASTUPDATE` date DEFAULT NULL,
  `TMCHECK` date DEFAULT NULL,
  `STRCHECKMAN` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NSEGINT` int(10) DEFAULT NULL,
  `NPGMINT` int(10) DEFAULT NULL,
  `NLISTLENGTH` int(10) DEFAULT NULL,
  `NLANMUID` int(10) NOT NULL DEFAULT '0',
  `NBANZIID` int(10) DEFAULT NULL,
  `NPLAYTIMES` int(10) DEFAULT NULL,
  `NVERSION` int(10) DEFAULT NULL,
  `NRESEAVE` int(10) DEFAULT NULL,
  `STRRESERVE` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NAIRSTATUS` int(10) DEFAULT NULL,
  `MOSROID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NLOCKUPDATE` int(10) DEFAULT NULL,
  `NTMFLAG` int(10) DEFAULT NULL,
  `NPLAYLISTTYPE` int(10) DEFAULT NULL,
  `STRDESC` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `BEDELETE` int(10) NOT NULL DEFAULT '0',
  `STUDIOID` int(10) NOT NULL DEFAULT '0',
  `VTRID` int(10) NOT NULL DEFAULT '0',
  `NRCS` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `COMPILEMARGIN` int(10) DEFAULT NULL,
  `STRROTRIGGER` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NSEGNUMBER` int(10) DEFAULT NULL,
  `NPGMNUMBER` int(10) DEFAULT NULL,
  UNIQUE KEY `U_NLISTID` (`ID`),
  KEY `N_NSTUDIOID` (`STUDIOID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_playlistformat`
--

DROP TABLE IF EXISTS `sbt_playlistformat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_playlistformat` (
  `PLID` int(10) NOT NULL,
  `HEADID` int(10) DEFAULT NULL,
  `HEADLENGTH` int(10) DEFAULT NULL,
  `GAPID` int(10) DEFAULT NULL,
  `GAPLENGTH` int(10) DEFAULT NULL,
  `REARID` int(10) DEFAULT NULL,
  `REARLENGTH` int(10) DEFAULT NULL,
  PRIMARY KEY (`PLID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_playlisttomsv`
--

DROP TABLE IF EXISTS `sbt_playlisttomsv`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_playlisttomsv` (
  `PLAYLISTID` int(10) NOT NULL,
  `MSVIP` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `LASTTIME` date NOT NULL,
  PRIMARY KEY (`PLAYLISTID`,`MSVIP`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_serverinfo`
--

DROP TABLE IF EXISTS `sbt_serverinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_serverinfo` (
  `SERVERID` int(10) NOT NULL,
  `SERVERIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `SERVERNAME` varchar(128) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `SERVERTYPE` int(10) DEFAULT NULL,
  `SUBTYPE` int(10) DEFAULT NULL,
  `WORKTYPE` int(10) DEFAULT NULL,
  `CLIPDIR` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `CHANNELINT` int(10) DEFAULT NULL,
  `STUDIOID` int(10) DEFAULT NULL,
  `SHAREUSE` int(10) DEFAULT NULL,
  `PORTBANDWIDTH` int(10) NOT NULL DEFAULT '0',
  `PORTCOUNT` int(10) NOT NULL DEFAULT '0',
  `TRANSTYPE` int(10) NOT NULL DEFAULT '0',
  `DESPATHPARAM` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `APPLYID` int(10) NOT NULL DEFAULT '0',
  `USETYPE` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`SERVERID`),
  UNIQUE KEY `U_SERVERNAME` (`SERVERNAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_sharetask`
--

DROP TABLE IF EXISTS `sbt_sharetask`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_sharetask` (
  `ITASKID` int(10) NOT NULL,
  `ISERVERID` int(10) NOT NULL,
  PRIMARY KEY (`ITASKID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_sourcefileinfo`
--

DROP TABLE IF EXISTS `sbt_sourcefileinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_sourcefileinfo` (
  `STRSOURCEID` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `STRFILENAME` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `NFILETYPE` int(10) DEFAULT NULL,
  `QUALITYTYPE` int(10) NOT NULL,
  PRIMARY KEY (`STRSOURCEID`,`STRFILENAME`,`QUALITYTYPE`),
  KEY `N_STRSOURCEID` (`STRSOURCEID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_sourceinfo`
--

DROP TABLE IF EXISTS `sbt_sourceinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_sourceinfo` (
  `STRSOURCEID` varchar(40) COLLATE utf8mb4_unicode_ci NOT NULL,
  `STRMATERIALNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NVERSION` int(10) DEFAULT NULL,
  `MOSROID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TMCREATETIME` date NOT NULL,
  `NATTRIBUTE` int(10) DEFAULT NULL,
  `STRRIGHTS` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRDSCPT` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NSTATUS` int(10) DEFAULT NULL,
  `NSOF` int(10) DEFAULT NULL,
  `NEOF` int(10) DEFAULT NULL,
  `NLENGTH` int(10) DEFAULT NULL,
  `LANGUAGE` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STANDARD` int(10) DEFAULT NULL,
  `CHECKSTATUS` int(10) DEFAULT NULL,
  `STRVERSION` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`STRSOURCEID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_storyinfoex`
--

DROP TABLE IF EXISTS `sbt_storyinfoex`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_storyinfoex` (
  `ID` int(10) NOT NULL,
  `STRSTORYID` varchar(128) COLLATE utf8mb4_unicode_ci NOT NULL,
  `NSTORYNO` int(10) NOT NULL,
  `STRLISTID` varchar(256) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NPLAYTYPE` int(10) DEFAULT NULL,
  `NSTORYLENGTH` int(10) DEFAULT NULL,
  `STRSTORYNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MOSROID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRPRODUCER` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRDESCPT` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRORIGIN` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRANNOUNCER` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRREPORTERS` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRLASTWORDS` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `NSTATUS` int(10) DEFAULT NULL,
  `STRCHECKMAN` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TMCREATETIME` date DEFAULT NULL,
  `NREFCOUNTER` int(10) DEFAULT NULL,
  `BEDELETE` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`STRSTORYID`),
  UNIQUE KEY `U_NSTROYIDEX` (`ID`),
  KEY `N_MOSROID` (`MOSROID`(191)),
  KEY `N_STRLISTID` (`STRLISTID`(191)),
  KEY `N_TMCREATETIME` (`TMCREATETIME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_studiocontrol`
--

DROP TABLE IF EXISTS `sbt_studiocontrol`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_studiocontrol` (
  `CONTROLID` int(10) NOT NULL,
  `STUDIOID` int(10) NOT NULL,
  PRIMARY KEY (`CONTROLID`,`STUDIOID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_studiohotkey`
--

DROP TABLE IF EXISTS `sbt_studiohotkey`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_studiohotkey` (
  `STUDIOID` int(10) NOT NULL,
  `PREVIEWEND` int(10) NOT NULL DEFAULT '0',
  `PREVIEWHOME` int(10) NOT NULL DEFAULT '0',
  `PREVIEWIN` int(10) NOT NULL DEFAULT '0',
  `PREVIEWNEXT` int(10) NOT NULL DEFAULT '0',
  `PREVIEWOUT` int(10) NOT NULL DEFAULT '0',
  `PREVIEWPLAY` int(10) NOT NULL DEFAULT '0',
  `PREVIEWSEEKA` int(10) NOT NULL DEFAULT '0',
  `PREVIEWSEEKB` int(10) NOT NULL DEFAULT '0',
  `PREVIEWVALUE` int(10) NOT NULL DEFAULT '1',
  `ISPREVIEWVALID` int(10) NOT NULL DEFAULT '0',
  `ISSHOWTIMEVALID` int(10) NOT NULL DEFAULT '0',
  `CHANNELPLAY` int(10) NOT NULL DEFAULT '0',
  `CHANNELNEXT` int(10) NOT NULL DEFAULT '0',
  `CHANNELTAKE` int(10) NOT NULL DEFAULT '0',
  `CHANNELPREVIOUS` int(10) NOT NULL DEFAULT '0',
  `ISCHANNELVALID` int(10) NOT NULL DEFAULT '0',
  `PREVIEWBACK` int(10) NOT NULL DEFAULT '0',
  `PREVIEWPAUSE` int(10) NOT NULL DEFAULT '0',
  `CHANNELPAUSE` int(10) NOT NULL DEFAULT '0',
  `CHANNELCONTINUE` int(10) DEFAULT NULL,
  PRIMARY KEY (`STUDIOID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_studioinfo`
--

DROP TABLE IF EXISTS `sbt_studioinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_studioinfo` (
  `NSTUDIOID` int(10) NOT NULL,
  `STRSTUDIONAME` varchar(128) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STUDIOCODE` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MOSSERVERIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MOSMQNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MOSSERVERIP_BAK` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MOSMQNAME_BAK` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ARCHIVESERVERIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ARCHIVESERVERMQNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MOSBACKMQ` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MAXCONNECT` int(10) NOT NULL DEFAULT '0',
  `FILESERVER` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `UDPPORT` int(10) DEFAULT NULL,
  `NSTATUS` int(10) DEFAULT NULL,
  `NUSERID` int(10) DEFAULT NULL,
  `TMUSESTARTTIME` date DEFAULT NULL,
  `NUSELASTTIME` int(10) DEFAULT NULL,
  `NUSECOLUMNID` int(10) DEFAULT NULL,
  `CARRYTYPE` int(10) DEFAULT NULL,
  `CHECKPGMWHILEPLAY` int(10) DEFAULT NULL,
  `PLAYENDMODE` int(10) DEFAULT NULL,
  `AUTOUPDATELIST` int(10) DEFAULT NULL,
  `SYNDELAY` int(10) DEFAULT NULL,
  `REPLACWCUED` int(10) DEFAULT NULL,
  `EDITMODE` int(10) DEFAULT NULL,
  `FOLLOWMODE` int(10) DEFAULT NULL,
  `SKIPNOCLIP` int(10) DEFAULT NULL,
  `WAITENDTM` int(10) DEFAULT NULL,
  `PREMARGIN` int(10) DEFAULT NULL,
  `POSTMARGIN` int(10) DEFAULT NULL,
  `SWITCHCHANNELMODE` int(10) DEFAULT NULL,
  `FORCESWITCHCHANNEL` int(10) DEFAULT NULL,
  `LOCKEND` int(10) DEFAULT NULL,
  `PERMITIPS` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `FORBIDIPS` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `CMDDELAYTIME` int(10) DEFAULT NULL,
  `OUTSTOPTYPE` int(10) DEFAULT NULL,
  `SHAREMODE` int(10) NOT NULL DEFAULT '0',
  `ADJUSTIP` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ADJUSTPORT` int(10) DEFAULT '0',
  `NCRID` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `CTRLTYPE` int(10) DEFAULT '0',
  `INTERFACEID` int(10) DEFAULT NULL,
  `INTERFACEPORT` int(10) DEFAULT '0',
  `ADJUSTPORTB` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  PRIMARY KEY (`NSTUDIOID`),
  UNIQUE KEY `U_STUDIONAME` (`STRSTUDIONAME`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_studioplayserver`
--

DROP TABLE IF EXISTS `sbt_studioplayserver`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_studioplayserver` (
  `SERVERID` int(10) NOT NULL,
  `STUDIOID` int(10) NOT NULL,
  PRIMARY KEY (`SERVERID`,`STUDIOID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_taskinfo`
--

DROP TABLE IF EXISTS `sbt_taskinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_taskinfo` (
  `TASKID` int(10) NOT NULL,
  `MATERIALNAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `SOURCEID` varchar(256) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `MATERIALID` varchar(128) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `GRAPHID` varchar(256) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TASKTYPE` int(10) DEFAULT NULL,
  `STRATEGYID` int(10) DEFAULT NULL,
  `STATUS` int(10) DEFAULT NULL,
  `SAVIFILENAME` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `SWAVFILENAMEL` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `SWAVFILENAMER` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `SSOURCEDIR` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `DLOCALDIR` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `DLOCALFILE` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `DLOCALWAVL` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `DLOCALWAVR` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `TMCREATEDAY` date DEFAULT NULL,
  `TMDELETEDAY` date DEFAULT NULL,
  `ISTUDIOID` int(10) DEFAULT NULL,
  `SOM` int(10) DEFAULT NULL,
  `EOM` int(10) DEFAULT NULL,
  `PRIORITY` int(10) NOT NULL DEFAULT '10',
  `PLAYTIME` date DEFAULT NULL,
  `NSTORYID` int(10) DEFAULT '0',
  `NVERSION` int(10) DEFAULT NULL,
  PRIMARY KEY (`TASKID`),
  UNIQUE KEY `U_TASKINFO` (`MATERIALID`,`ISTUDIOID`,`STRATEGYID`,`TASKTYPE`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_usercontrol1`
--

DROP TABLE IF EXISTS `sbt_usercontrol1`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_usercontrol1` (
  `IUSERID` int(11) DEFAULT NULL,
  `ITEMP1` int(10) DEFAULT NULL,
  `ITEMP2` int(10) DEFAULT NULL,
  `STRTEMP1` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `STRTEMP2` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `ITEMP3` int(11) DEFAULT NULL,
  `STRTEMP3` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_userhotkey`
--

DROP TABLE IF EXISTS `sbt_userhotkey`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_userhotkey` (
  `USERID` int(10) NOT NULL,
  `PREVIEWEND` int(10) NOT NULL DEFAULT '0',
  `PREVIEWHOME` int(10) NOT NULL DEFAULT '0',
  `PREVIEWIN` int(10) NOT NULL DEFAULT '0',
  `PREVIEWNEXT` int(10) NOT NULL DEFAULT '0',
  `PREVIEWOUT` int(10) NOT NULL DEFAULT '0',
  `PREVIEWPLAY` int(10) NOT NULL DEFAULT '0',
  `ISPREVIEWVALID` int(10) NOT NULL DEFAULT '0',
  `ISSHOWTIMEVALID` int(10) NOT NULL DEFAULT '0',
  `CHANNELPLAY` int(10) NOT NULL DEFAULT '0',
  `CHANNELNEXT` int(10) NOT NULL DEFAULT '0',
  `CHANNELTAKE` int(10) NOT NULL DEFAULT '0',
  `CHANNELPREVIOUS` int(10) NOT NULL DEFAULT '0',
  `ISCHANNELVALID` int(10) NOT NULL DEFAULT '0',
  `PREVIEWBACK` int(10) NOT NULL DEFAULT '0',
  `PREVIEWPAUSE` int(10) NOT NULL DEFAULT '0',
  `CHANNELPAUSE` int(10) NOT NULL DEFAULT '0',
  `PREVIEWSEEKA` int(10) NOT NULL DEFAULT '0',
  `PREVIEWSEEKB` int(10) NOT NULL DEFAULT '0',
  `PREVIEWVALUE` int(10) NOT NULL DEFAULT '0',
  `CHANNELCONTINUE` int(10) NOT NULL,
  PRIMARY KEY (`USERID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_userinstudio`
--

DROP TABLE IF EXISTS `sbt_userinstudio`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_userinstudio` (
  `USERID` int(10) NOT NULL,
  `STUDIOID` int(10) NOT NULL,
  `USETYPE` int(10) NOT NULL DEFAULT '0',
  PRIMARY KEY (`USERID`,`STUDIOID`,`USETYPE`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbt_userparam`
--

DROP TABLE IF EXISTS `sbt_userparam`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `sbt_userparam` (
  `USERID` int(10) NOT NULL,
  `USERTYPE` int(10) DEFAULT NULL,
  `CHANNEL` int(10) DEFAULT NULL,
  `TCCOUNTTYPE` int(10) DEFAULT NULL,
  `SHOWPGMTYPE` int(10) DEFAULT NULL,
  `LISTPLAYTYPE` int(10) DEFAULT NULL,
  `AUTOSCROLLMODE` int(10) DEFAULT NULL,
  `ISAUTOSCROLL` int(10) DEFAULT NULL,
  `SHOWTIMETYPE` int(10) DEFAULT NULL,
  `NOPLAYCLR` int(10) DEFAULT NULL,
  `CUECLR` int(10) DEFAULT NULL,
  `CUEDCLR` int(10) DEFAULT NULL,
  `STANDBYCLR` int(10) DEFAULT NULL,
  `PLAYCLR` int(10) DEFAULT NULL,
  `PLAYEDCLR` int(10) DEFAULT NULL,
  `WARNCLR` int(10) DEFAULT NULL,
  `SKIPCLR` int(10) DEFAULT NULL,
  `BREAKCLR` int(10) DEFAULT NULL,
  `MPLAYINGCLR` int(10) DEFAULT NULL,
  `SPLAYINGCLR` int(10) DEFAULT NULL,
  `MPAUSEDCLR` int(10) DEFAULT NULL,
  `SPAUSEDCLR` int(10) DEFAULT NULL,
  `STOPEDCLR` int(10) DEFAULT NULL,
  `FONT` varchar(512) COLLATE utf8mb4_unicode_ci DEFAULT NULL,
  `FRESHTYPE` int(10) DEFAULT NULL,
  `PLAYCYCLE` int(10) DEFAULT '0',
  PRIMARY KEY (`USERID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2017-03-16 14:25:38

-- MySQL dump 10.13  Distrib 5.6.17, for Win64 (x86_64)
--
-- Host: 172.16.168.205    Database: studiodb
-- ------------------------------------------------------
-- Server version	5.6.28-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Dumping data for table `sbt_basedatainfo`
--

LOCK TABLES `sbt_basedatainfo` WRITE;
/*!40000 ALTER TABLE `sbt_basedatainfo` DISABLE KEYS */;
INSERT INTO `sbt_basedatainfo` VALUES ('AutoClearRecycle','0',1),('AUTODEL_CarryClipIP','172.16.1.8',0),('AUTODEL_CarryClipPort','6000',0),('AUTODEL_MaterialDeleteTime','10:00:00',0),('AUTODEL_MaterialEndDeleteTime','23:00:00',0),('AUTODEL_MaterialLifeDays','1000',0),('AUTODEL_MAXEXETASK','20',0),('AUTODEL_PlaylistDeleteTime','9:30:00',0),('AUTODEL_PlaylistLifeDays','1000',0),('CHLCLTPORT','9527',0),('CHLSVRPORT','8527',0),('DefaultClipBitmapDir','C:stamproot',0),('DeleteBlankEvent','1',1),('DELETEPOLICYID','6',0),('EVENTVERSIONTYPE','1',0),('HiveURL','http://172.16.131.154:88',0),('PNS','172.16.160.7',0),('Standard','8',0),('StudioChannel','10',0),('SUBTRANSPOLICYID','9',0),('TRANSPOLICYID','4',0);
/*!40000 ALTER TABLE `sbt_basedatainfo` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2017-03-16 14:26:10
