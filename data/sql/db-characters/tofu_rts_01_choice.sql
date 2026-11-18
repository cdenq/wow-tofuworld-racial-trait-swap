-- wow-tofuworld-racial-trait-swap: stores the race each swapped character chose.
-- Schema: acore_characters. Applied by the worldserver's updater as module SQL (state MODULE).
-- One row per character that swapped to another race's racials; no row means the character's own racials. The module deletes the row when the character is deleted or swaps back.
-- Re-runnable: CREATE TABLE IF NOT EXISTS.
-- Check: SHOW TABLES LIKE 'tofu_racial_swap';  -- 1 row

CREATE TABLE IF NOT EXISTS `tofu_racial_swap` (
  `guid` INT UNSIGNED NOT NULL COMMENT 'characters.guid',
  `race` TINYINT UNSIGNED NOT NULL COMMENT 'Race whose racial traits the character uses',
  PRIMARY KEY (`guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci COMMENT='Tofu racial trait swap choices';
