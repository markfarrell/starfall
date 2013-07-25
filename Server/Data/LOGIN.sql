/*
Navicat SQLite Data Transfer

Source Server         : Game Server
Source Server Version : 30706
Source Host           : :0

Target Server Type    : SQLite
Target Server Version : 30706
File Encoding         : 65001

Date: 2013-04-30 08:24:20
*/

PRAGMA foreign_keys = OFF;

-- ----------------------------
-- Table structure for "main"."LOGIN"
-- ----------------------------
DROP TABLE "main"."LOGIN";
CREATE TABLE "LOGIN" (
"USERID"  INTEGER NOT NULL,
"USERTYPE"  INTEGER NOT NULL DEFAULT 0,
"USERNAME"  TEXT NOT NULL,
"PASSWORD"  TEXT NOT NULL,
PRIMARY KEY ("USERID" ASC)
);

-- ----------------------------
-- Records of LOGIN
-- ----------------------------
INSERT INTO "main"."LOGIN" VALUES (1, 0, 'admin', 'admin');
