#include "SSHClientManager.h"

void SSHClientManager::startSSHClient(const SshConfig &config)
{
    if (clients.contains(config.name))
    {
        restartSSHClientWithConfig(config.name);
        return;
    }

    SSHClient *client = new SSHClient(logger);
    QThread *thread = new QThread();

    client->moveToThread(thread);

    connect(thread, &QThread::started, [client, config]() {
        client->connectToHost(config);
    });
    connect(client, &SSHClient::sshDataReceived, this, &SSHClientManager::forwardSSHData);

    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();

    clients[config.name] = qMakePair(client, thread);
}

void SSHClientManager::restartAllSSHClients()
{
    for (const QString &name : clients.keys())
    {
        restartSSHClientWithConfig(name);
    }
}

void SSHClientManager::restartSSHClientWithConfig(const QString &name)
{
    logger->log("restartSSHClientWithConfig name:" + name);

    if (clients.contains(name))
    {
        auto clientThreadPair = clients[name];
        SSHClient *client = clientThreadPair.first;
        QThread *thread = clientThreadPair.second;
        client->stopLooping(); // 调用停止循环的函数

        QThread::msleep(1000);


        disconnect(client, &SSHClient::sshDataReceived, this, &SSHClientManager::forwardSSHData);

        thread->quit();
        thread->wait();
        client->deleteLater();
        thread->deleteLater();

        clients.remove(name);

        // 重新解析配置文件
        SSHConfigParser parser;
        if (parser.loadFromFile(configFilePath))
        {
            // 在重新解析后获取最新的配置
            for (const SshConfig &config : parser.configs)
            {
                if (config.name == name)
                {
                    startSSHClient(config);
                     QThread::msleep(2000);
                    break;
                }
            }
        }
        else
        {
            logger->log("Failed to reload config file: " + configFilePath);
        }
    }
}

void SSHClientManager::loadConfigsAndStartClients(const QString &configFilePath)
{
    this->configFilePath = configFilePath;
    if (parser.loadFromFile(configFilePath))
    {
        for (const SshConfig &config : parser.configs)
        {
            startSSHClient(config);
        }
    }
    else
    {
        logger->log("Failed to load config file: " + configFilePath);
    }
}

SshConfig SSHClientManager::getConfigByName(const QString &name) const
{
    for (const SshConfig &config : parser.configs)
    {
        if (config.name == name)
        {
            return config;
        }
    }
    return SshConfig(); // 如果找不到配置，则返回一个空配置
}
