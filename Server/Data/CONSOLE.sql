

/*
Navicat SQLite Data Transfer

Source Server         : Game Server
Source Server Version : 30706
Source Host           : :0

Target Server Type    : SQLite
Target Server Version : 30706
File Encoding         : 65001

Date: 2013-05-18 10:47:08
*/

PRAGMA foreign_keys = OFF;

-- ----------------------------
-- Table structure for "main"."CONSOLE"
-- ----------------------------
DROP TABLE "main"."CONSOLE";
CREATE TABLE "CONSOLE" (
"ID"  INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL DEFAULT 0,
"MESSAGE"  TEXT NOT NULL
);

-- ----------------------------
-- Records of CONSOLE
-- ----------------------------
