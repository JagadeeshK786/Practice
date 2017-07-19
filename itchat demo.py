import itchat

@itchat.msg_register(itchat.content.TEXT)
def text_reply(msg):
    return "好的，我知道了。"

itchat.auto_login()
itchat.run()