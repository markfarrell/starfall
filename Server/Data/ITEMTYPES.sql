/*
Navicat SQLite Data Transfer

Source Server         : Game Server
Source Server Version : 30714
Source Host           : :0

Target Server Type    : SQLite
Target Server Version : 30714
File Encoding         : 65001

Date: 2013-05-26 10:05:47
*/

PRAGMA foreign_keys = OFF;

-- ----------------------------
-- Table structure for ITEMTYPES
-- ----------------------------
DROP TABLE IF EXISTS "main"."ITEMTYPES";
CREATE TABLE "ITEMTYPES" (
"ID"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL DEFAULT 0,
"PROPERTIES"  TEXT NOT NULL
);
