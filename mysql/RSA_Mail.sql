-- phpMyAdmin SQL Dump
-- version 4.6.6deb5
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Erstellungszeit: 31. Mai 2021 um 20:13
-- Server-Version: 10.3.27-MariaDB-0+deb10u1
-- PHP-Version: 7.3.27-1~deb10u1

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Datenbank: `RSA_Mail`
--

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `Messages`
--

CREATE TABLE `Messages` (
  `ID` int(11) NOT NULL,
  `From` int(11) NOT NULL,
  `To` int(11) NOT NULL,
  `Caption` text NOT NULL,
  `Message` longtext NOT NULL,
  `Signature` mediumtext NOT NULL,
  `Read` tinyint(1) NOT NULL DEFAULT 0,
  `SenderCaption` text NOT NULL,
  `SenderMessage` longtext NOT NULL,
  `Date` text NOT NULL,
  `ReceiveDate` text NOT NULL DEFAULT '0000-00-00 00:00:00',
  `SenderDate` text NOT NULL DEFAULT '0000-00-00',
  `SenderReceiveDate` text NOT NULL DEFAULT '0000-00-00'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 ROW_FORMAT=COMPACT;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `Users`
--

CREATE TABLE `Users` (
  `ID` int(11) NOT NULL,
  `Name` text NOT NULL,
  `E` text NOT NULL,
  `N` text NOT NULL,
  `Password` text NOT NULL,
  `Email` text DEFAULT 'None'
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 ROW_FORMAT=COMPACT;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `Versioninfo`
--

CREATE TABLE `Versioninfo` (
  `ID` int(11) NOT NULL,
  `Version` int(11) NOT NULL,
  `BinWin` longblob DEFAULT NULL,
  `BinLinux` longblob DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Indizes der exportierten Tabellen
--

--
-- Indizes für die Tabelle `Messages`
--
ALTER TABLE `Messages`
  ADD PRIMARY KEY (`ID`);

--
-- Indizes für die Tabelle `Users`
--
ALTER TABLE `Users`
  ADD PRIMARY KEY (`ID`);

--
-- Indizes für die Tabelle `Versioninfo`
--
ALTER TABLE `Versioninfo`
  ADD PRIMARY KEY (`ID`);

--
-- AUTO_INCREMENT für exportierte Tabellen
--

--
-- AUTO_INCREMENT für Tabelle `Messages`
--
ALTER TABLE `Messages`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;
--
-- AUTO_INCREMENT für Tabelle `Users`
--
ALTER TABLE `Users`
  MODIFY `ID` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
