use [2016184041_gs]
GO

SET ANSI_NULLS ON
GO
	SET NOCOUNT ON
GO
SET QUOTED_IDENTIFIER ON
GO

IF EXISTS ( SELECT * FROM INFORMATION_SCHEMA.TABLES  WHERE TABLE_NAME = 'user_table' )
	DROP TABLE [dbo].[user_table]
GO

CREATE TABLE [dbo].[user_table]
(
	[user_id] [int] NOT NULL PRIMARY KEY IDENTITY(100, 1),
	[user_name] [NVARCHAR](50) NOT NULL DEFAULT (N'default'),
	[current_pos_x] [int] NOT NULL DEFAULT (1),
	[current_pos_y] [int] NOT NULL DEFAULT (1),
	[current_level] [int] NOT NULL DEFAULT ((1)),
	[current_hp] [int] NOT NULL DEFAULT ((200)),
	[current_exp] [int] NOT NULL DEFAULT ((0)),
)

GO


IF EXISTS ( SELECT * FROM sys.procedures WHERE NAME ='load_user' )
	DROP PROCEDURE [dbo].[load_user]
GO
CREATE PROCEDURE [dbo].[load_user]
	@name NVARCHAR(50)
AS
BEGIN
	SET NOCOUNT ON
	SELECT user_id, user_name, current_pos_y, current_pos_x, current_level, current_exp, current_hp
	FROM [dbo].[user_table] WHERE user_name = @name
END
GO



IF EXISTS ( SELECT * FROM sys.procedures WHERE NAME ='update_user_position' )
	DROP PROCEDURE [dbo].[update_user_position]
GO
CREATE PROCEDURE [dbo].[update_user_position]
	@user_id INT,
	@user_y INT,
	@user_x INT
AS
BEGIN
	SET NOCOUNT ON;
	UPDATE [dbo].[user_table]
	SET current_pos_x = @user_x, current_pos_y = @user_y
	WHERE user_id = @user_id;
	SELECT @@ROWCOUNT;
END
GO


IF EXISTS ( SELECT * FROM sys.procedures WHERE NAME ='update_user_all' )
	DROP PROCEDURE [dbo].[update_user_all]
GO
CREATE PROCEDURE [dbo].[update_user_all]
	@user_id INT,
	@user_y INT,
	@user_x INT,
	@user_hp INT,
	@user_level INT,
	@user_exp INT
AS
BEGIN
	SET NOCOUNT ON;
	UPDATE [dbo].[user_table]
	SET current_pos_x = @user_x, current_pos_y = @user_y, current_exp = @user_exp, current_level = @user_level, current_hp = @user_hp
	WHERE user_id = @user_id;
	SELECT @@ROWCOUNT;
END
GO

IF EXISTS ( SELECT * FROM sys.procedures WHERE NAME ='update_user_stat' )
	DROP PROCEDURE [dbo].[update_user_stat]
GO
CREATE PROCEDURE [dbo].[update_user_stat]
	@user_id INT,
	@user_hp INT,
	@user_level INT,
	@user_exp INT
AS
BEGIN
	SET NOCOUNT ON;
	UPDATE [dbo].[user_table]
	SET current_exp = @user_exp, current_level = @user_level, current_hp = @user_hp
	WHERE user_id = @user_id;
	SELECT @@ROWCOUNT;
END
GO



IF EXISTS ( select * from sys.procedures where name='create_user' )
	DROP PROCEDURE [dbo].[create_user]
GO

CREATE PROCEDURE [dbo].[create_user]
	@name	NVARCHAR(10),
	@posx	INT,
	@posy	INT
AS
BEGIN
	SET NOCOUNT ON;
	INSERT INTO user_table( user_name, current_pos_y, current_pos_x) VALUES (@name, @posy, @posx);
	SELECT @@IDENTITY 
END
GO





--EXEC load_user "user_5"
--GO

--EXEC update_user_position 0, 1, 99
--GO

--DECLARE @COUNTER	INT
--DECLARE @name		CHAR(50)
--SET @COUNTER = 0;

	--EXEC create_user "tt"
	--GO

--WHILE @COUNTER < 100
--BEGIN
--	SET NOCOUNT ON;
--	SET @name = CONCAT('bot_', @counter);
--	EXEC create_user @name
--	SET @COUNTER = @COUNTER + 1;
--END

SELECT * FROM user_table;
GO